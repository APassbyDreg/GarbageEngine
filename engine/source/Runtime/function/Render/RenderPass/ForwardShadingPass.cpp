#include "ForwardShadingPass.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "Runtime/function/Scene/Components/Material.h"
#include "Runtime/function/Scene/Components/Mesh.h"
#include "Runtime/function/Scene/Components/Transform.h"

#include "../Shared/LightUniform.h"
#include "../Shared/ViewUniform.h"

#include "../VulkanManager/RenderUtils.h"

#include "../Manager/PerSceneDataManager.h"
#include "../Manager/PerViewDataManager.h"

namespace GE
{

    OpaqueForwardShadingPassUnit::OpaqueForwardShadingPassUnit(GraphicsPass&                    pass,
                                                               std::shared_ptr<Mesh>            mesh,
                                                               std::shared_ptr<ForwardMaterial> material) :
        GraphicsPassUnit(pass),
        m_mesh(mesh), m_material(material)
    {
        std::string pipeline_key = std::format("OpaqueForwardShading-{}-{}", mesh->GetType(), material->GetType());
        if (RenderPipelineManager::HasGraphicsPipeline(pipeline_key))
        {
            m_pipeline = RenderPipelineManager::GetGraphicsPipeline(pipeline_key);
        }
        else
        {
            auto& render_pass = pass.GetRenderPassObject();
            m_pipeline        = std::make_shared<GraphicsRenderPipeline>();

            // per scene data
            PerSceneDataManager::SetupPipeline(*m_pipeline);
            // per view data
            PerViewDataManager::SetupPipeline(*m_pipeline);
            // states
            m_pipeline->m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();
            VkViewport viewport            = VkInit::GetViewport(pass.GetExtent());
            VkRect2D   scissor             = {{0, 0}, pass.GetExtent()};
            m_pipeline->m_viewportState    = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);
            m_pipeline->m_colorBlendState =
                VkInit::GetPipelineColorBlendStateCreateInfo(render_pass.GetFlattenColorBlendStates());
            m_pipeline->m_dynamicStates     = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            m_pipeline->m_depthStencilState = VkInit::GetPipelineDepthStencilStateCreateInfo(
                render_pass.enableDepthStencil, render_pass.enableDepthStencil);

            // mesh and material
            mesh->SetupRenderPipeline(*m_pipeline);
            material->SetupRenderPipeline(*m_pipeline);

            // build
            m_pipeline->Build(pass.GetRenderPass());
            RenderPipelineManager::RegisterGraphicsPipeline(pipeline_key, m_pipeline);
        }

        mesh->SetupPassResources(*this);
        material->SetupPassResources(*this);
    }

    void OpaqueForwardShadingPassUnit::Run(uint                                 frame_idx,
                                           VkCommandBuffer                      cmd,
                                           std::vector<std::shared_ptr<Entity>> instances)
    {
        auto& resource_manager = m_pass.GetResourceManager();
        {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

            // set dynamic viewport
            VkViewport viewport = VkInit::GetViewport(m_pass.GetExtent());
            VkRect2D   scissor  = {{0, 0}, m_pass.GetExtent()};
            vkCmdSetViewport(cmd, 0, 1, &viewport);
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            // bind per scene data
            auto&& per_scene_desc_sets = resource_manager.GetPerFramePersistantDescriptorSet(frame_idx, "PerSceneData");
            vkCmdBindDescriptorSets(cmd,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline->GetPipelineLayout(),
                                    0,
                                    1,
                                    &per_scene_desc_sets,
                                    0,
                                    nullptr);

            // bind view uniform
            auto&& per_view_desc_sets =
                resource_manager.GetPerFramePersistantDescriptorSet(frame_idx, "PerViewData/MainCamera");
            vkCmdBindDescriptorSets(cmd,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline->GetPipelineLayout(),
                                    1,
                                    1,
                                    &per_view_desc_sets,
                                    0,
                                    nullptr);

            // bind material data
            auto&& layout = m_pipeline->GetPipelineLayout();
            {
                MaterialRenderPassData data = {frame_idx, cmd, *this};
                m_material.lock()->RunShadingPass(data);
            }

            // bind mesh data and dispatch
            {
                MeshRenderPassData data = {frame_idx, cmd, instances, *this};
                m_mesh.lock()->RunRenderPass(data);
            }
        }
    }

    void OpaqueForwardShadingPass::Init(uint frame_cnt)
    {
        m_frameCnt = frame_cnt;

        // build render pass
        {
            // color
            GraphicsColorResource color_output = {};
            color_output.desc                  = VkInit::GetAttachmentDescription();
            color_output.blend                 = VkInit::GetPipelineColorBlendAttachmentState();
            m_renderPass.output.push_back(color_output);
            // depth
            m_renderPass.enableDepthStencil = true;
            m_renderPass.depthAttachment    = VkInit::GetAttachmentDescription(VK_FORMAT_D32_SFLOAT);
            // build
            m_renderPass.Build();
        }
    }

    void OpaqueForwardShadingPass::Resize(uint width, uint height)
    {
        m_extent = {width, height};
        m_frameBuffers.clear();
        auto&& color_imgs = m_resourceManager.GetFramewiseImage("ColorRT");
        auto&& depth_imgs = m_resourceManager.GetFramewiseImage("DepthRT");
        for (uint frame_id = 0; frame_id < m_frameCnt; frame_id++)
        {
            auto&& color_view_info =
                VkInit::GetVkImageViewCreateInfo(color_imgs[frame_id]->GetImageInfo(), VK_IMAGE_ASPECT_COLOR_BIT);
            auto&& depth_view_info =
                VkInit::GetVkImageViewCreateInfo(depth_imgs[frame_id]->GetImageInfo(), VK_IMAGE_ASPECT_DEPTH_BIT);
            std::vector<VkImageView> views            = {color_imgs[frame_id]->GetImageView(color_view_info),
                                                         depth_imgs[frame_id]->GetImageView(depth_view_info)};
            VkFramebufferCreateInfo  framebuffer_info = {};
            framebuffer_info.sType                    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass               = GetRenderPass();
            framebuffer_info.attachmentCount          = views.size();
            framebuffer_info.pAttachments             = views.data();
            framebuffer_info.width                    = width;
            framebuffer_info.height                   = height;
            framebuffer_info.layers                   = 1;
            m_frameBuffers.push_back(std::make_shared<FrameBuffer>(framebuffer_info));
        }
    }

    void OpaqueForwardShadingPass::Run(RenderPassRunData run_data, OpaqueForwardShadingPassData pass_data)
    {
        auto&& [frame_idx, cmd] = run_data;
        auto&& [renderables]    = pass_data;

        // begin render pass
        {
            VkRenderPassBeginInfo rp_info = {};
            rp_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_info.renderPass            = GetRenderPass();
            rp_info.framebuffer           = m_frameBuffers[frame_idx]->Get();
            rp_info.renderArea.offset     = {0, 0};
            rp_info.renderArea.extent     = m_extent;
            rp_info.clearValueCount       = 0;
            rp_info.pClearValues          = nullptr;
            vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
        }

        for (auto&& [k, instances] : renderables)
        {
            auto mesh     = instances[0]->GetComponent<MeshComponent>().GetCoreValue();
            auto material = std::dynamic_pointer_cast<ForwardMaterial>(
                instances[0]->GetComponent<MaterialComponent>().GetCoreValue());

            // create a unit if needed
            if (!StdUtils::Exists(m_units, k))
            {
                m_units[k] = std::make_shared<OpaqueForwardShadingPassUnit>(*this, mesh, material);
            }

            m_units[k]->Run(frame_idx, cmd, instances);
        }

        // end render pass
        {
            vkCmdEndRenderPass(cmd);
        }
    }
} // namespace GE