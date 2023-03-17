#include "ForwardShadingPass.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "Runtime/function/Scene/Components/Material.h"
#include "Runtime/function/Scene/Components/Mesh.h"
#include "Runtime/function/Scene/Components/Transform.h"

#include "../Shared/LightUniform.h"
#include "../Shared/ViewUniform.h"

#include "../VulkanManager/RenderUtils.h"

#include "../Manager/PerSceneDataManager.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    // void ForwardShadingPass::Init(uint frame_cnt)
    // {
    //     m_name = "ForwardShadingPass";

    //     /* ---------------------- setup renderpass ---------------------- */
    //     if (!s_renderPass.IsBuilt())
    //     {
    //         // color
    //         GraphicsColorResource color_output = {};
    //         color_output.desc                  = VkInit::GetAttachmentDescription();
    //         color_output.blend                 = VkInit::GetPipelineColorBlendAttachmentState();
    //         s_renderPass.output.push_back(color_output);
    //         // depth
    //         s_renderPass.enableDepthStencil = true;
    //         s_renderPass.depthAttachment    = VkInit::GetAttachmentDescription(VK_FORMAT_D32_SFLOAT);
    //         // build
    //         s_renderPass.Build();
    //     }

    //     /* ------------------------- setup pipeline ------------------------- */
    //     if (m_pipeline == nullptr)
    //     {
    //         std::string pipeline_name = std::format("ForwardShading-{}-{}", m_meshName, m_materialName);
    //         if (RenderPipelineManager::HasGraphicsPipeline(pipeline_name))
    //         {
    //             m_pipeline = RenderPipelineManager::GetGraphicsPipeline(pipeline_name);
    //         }
    //         else
    //         {
    //             m_pipelineSetupFns.push_back([=](GraphicsRenderPipeline& pipeline) {
    //                 // per scene data
    //                 PerSceneDataManager::SetupPipeline(pipeline);
    //                 // per view data
    //                 pipeline.AddDescriptorSetLayoutBinding(1, ViewUniform::GetDescriptorSetLayoutBinding());
    //                 // states
    //                 pipeline.m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();
    //                 VkViewport viewport         = VkInit::GetViewport(m_extent);
    //                 VkRect2D   scissor          = {{0, 0}, m_extent};
    //                 pipeline.m_viewportState    = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);
    //                 pipeline.m_colorBlendState =
    //                     VkInit::GetPipelineColorBlendStateCreateInfo(s_renderPass.GetFlattenColorBlendStates());
    //                 pipeline.m_dynamicStates     = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    //                 pipeline.m_depthStencilState = VkInit::GetPipelineDepthStencilStateCreateInfo(
    //                     s_renderPass.enableDepthStencil, s_renderPass.enableDepthStencil);
    //                 //    VK_COMPARE_OP_GREATER); // inverse z
    //             });
    //             m_pipeline = BuildPipeline();
    //             RenderPipelineManager::RegisterGraphicsPipeline(pipeline_name, m_pipeline);
    //         }
    //     }

    //     /* -------------------- run post build functions -------------------- */
    //     for (auto&& fn : m_passSetupFns)
    //     {
    //         fn(*this);
    //     }
    // }

    // void ForwardShadingPass::Run(RenderPassRunData run_data, ForwardShadingPassData pass_data)
    // {
    //     // unpack data
    //     auto&& [frame_idx, cmd]                           = run_data;
    //     auto&& [viewport_size, instances, mesh, material] = pass_data;

    //     // begin render pass
    //     {
    //         VkRenderPassBeginInfo rp_info = {};
    //         rp_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //         rp_info.renderPass            = GetRenderPass();
    //         rp_info.framebuffer           = m_frameBuffers[frame_idx]->Get();
    //         rp_info.renderArea.offset     = {0, 0};
    //         rp_info.renderArea.extent     = m_extent;
    //         rp_info.clearValueCount       = 0;
    //         rp_info.pClearValues          = nullptr;
    //         vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
    //     }

    //     // bind data and draw
    //     {
    //         vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());

    //         // set dynamic viewport
    //         VkViewport viewport = VkInit::GetViewport(viewport_size);
    //         VkRect2D   scissor  = {{0, 0}, viewport_size};
    //         vkCmdSetViewport(cmd, 0, 1, &viewport);
    //         vkCmdSetScissor(cmd, 0, 1, &scissor);

    //         // bind per scene data
    //         auto&& per_scene_desc_sets =
    //             m_resourceManager.GetPerFramePersistantDescriptorSet(frame_idx, "PerSceneData");
    //         vkCmdBindDescriptorSets(
    //             cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipelineLayout(), 0, 1, &per_scene_desc_sets, 0, nullptr);

    //         // bind view uniform
    //         auto&& per_view_desc_sets =
    //             m_resourceManager.GetPerFramePersistantDescriptorSet(frame_idx, "ViewUniform/MainCamera");
    //         vkCmdBindDescriptorSets(
    //             cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipelineLayout(), 1, 1, &per_view_desc_sets, 0, nullptr);

    //         // bind material data
    //         auto&& layout = m_pipeline->GetPipelineLayout();
    //         {
    //             MaterialRenderPassData data = {frame_idx, cmd, *this};
    //             material->RunShadingPass(data);
    //         }

    //         // bind mesh data and dispatch
    //         {
    //             MeshRenderPassData data = {frame_idx, cmd, layout, instances, *this};
    //             mesh->RunRenderPass(data);
    //         }
    //     }

    //     // end render pass
    //     {
    //         vkCmdEndRenderPass(cmd);
    //     }
    // }

    // void CombinedForwardShadingPass::Resize(uint width, uint height)
    // {
    //     for (auto&& [key, pass] : m_passes)
    //     {
    //         pass->Resize(width, height);
    //     }
    //     m_size = {width, height};
    // }

    // void CombinedForwardShadingPass::Run(RenderPassRunData run_data, CombinedForwardShadingPassData pass_data)
    // {
    //     auto&& [frame_idx, cmd] = run_data;
    //     auto&& [renderables]    = pass_data;

    //     uint num_dispatched = 0;
    //     uint total_dispatch = renderables.size();
    //     for (auto&& [k, instances] : renderables)
    //     {
    //         num_dispatched++;
    //         auto mesh     = instances[0]->GetComponent<MeshComponent>().GetCoreValue();
    //         auto material = std::dynamic_pointer_cast<ForwardMaterial>(
    //             instances[0]->GetComponent<MaterialComponent>().GetCoreValue());

    //         // create pass if needed
    //         if (!StdUtils::Exists(m_passes, k))
    //         {
    //             auto&& color_targets = m_resourceManager.GetFramewiseImage("ColorRT");
    //             auto&& depth_targets = m_resourceManager.GetFramewiseImage("DepthRT");
    //             m_passes[k]          = std::make_shared<ForwardShadingPass>(
    //                 mesh, material, color_targets, depth_targets, m_resourceManager);
    //             m_passes[k]->Init(m_framesParallel);
    //             m_passes[k]->Resize(m_size.width, m_size.height);
    //         }

    //         /* ------------------------ dispatch pass ----------------------- */
    //         auto&&                 pass      = m_passes[k];
    //         ForwardShadingPassData pass_data = {
    //             m_size, instances, mesh, std::dynamic_pointer_cast<ForwardMaterial>(material)};
    //         pass->Run(run_data, pass_data);
    //     }
    // }

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
            m_pipeline->AddDescriptorSetLayoutBinding(1, ViewUniform::GetDescriptorSetLayoutBinding());
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
                resource_manager.GetPerFramePersistantDescriptorSet(frame_idx, "ViewUniform/MainCamera");
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
            std::vector<VkImageView> views            = {color_imgs[frame_id]->GetImageView(),
                                                         depth_imgs[frame_id]->GetImageView()};
            VkFramebufferCreateInfo  framebuffer_info = {};
            framebuffer_info.sType                    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass               = GetRenderPass();
            framebuffer_info.attachmentCount          = views.size();
            framebuffer_info.pAttachments             = views.data();
            framebuffer_info.width                    = width;
            framebuffer_info.height                   = height;
            framebuffer_info.layers                   = 1;
            m_frameBuffers.emplace_back(std::make_shared<FrameBuffer>(framebuffer_info));
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