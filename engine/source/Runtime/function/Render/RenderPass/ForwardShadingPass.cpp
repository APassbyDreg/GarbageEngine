#include "ForwardShadingPass.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "Runtime/function/Scene/Components/Material.h"
#include "Runtime/function/Scene/Components/Mesh.h"
#include "Runtime/function/Scene/Components/Transform.h"

#include "../Shared/ViewUniform.h"

#include "../VulkanManager/RenderUtils.h"
#include "glm/matrix.hpp"
#include "vulkan/vulkan_core.h"

namespace GE
{
    void ForwardShadingPass::Resize(uint width, uint height)
    {
        m_extent = {width, height};

        m_frameBuffers.clear();
        for (uint frame_id = 0; frame_id < m_numFrames; frame_id++)
        {
            // (re)create frame buffer
            auto&& color_img = m_colorTargets[frame_id];
            auto&& depth_img = m_depthTargets[frame_id];
            // these images are already resized
            VkFramebufferCreateInfo framebuffer_info = {};
            VkImageView             img_views[]      = {color_img->GetImageView(), depth_img->GetImageView()};
            framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass              = GetRenderPass();
            framebuffer_info.attachmentCount         = 2;
            framebuffer_info.pAttachments            = img_views;
            framebuffer_info.width                   = width;
            framebuffer_info.height                  = height;
            framebuffer_info.layers                  = 1;
            m_frameBuffers.emplace_back(std::make_shared<FrameBuffer>(framebuffer_info));
        }
    }

    void ForwardShadingPass::InitInternal(uint frame_cnt)
    {
        m_name = "ForwardShadingPass";

        /* ------------------------ reserve resources ----------------------- */

        /* ------------------------- setup resources ------------------------ */
        // color
        GraphicsColorResource color_output = {};
        color_output.desc                  = VkInit::GetAttachmentDescription();
        m_output.push_back(color_output);
        // depth
        m_enableDepthStencil = true;
        m_depthAttachment    = VkInit::GetAttachmentDescription(VK_FORMAT_D32_SFLOAT);
        __update_resource();

        /* ------------------------- setup pipeline ------------------------- */
        m_pipelineSetupFunc(m_pipeline);
        // states
        m_pipeline.m_multisampleState = VkInit::GetPipelineMultisampleStateCreateInfo();
        VkViewport viewport           = VkInit::GetViewport(m_extent);
        VkRect2D   scissor            = {{0, 0}, m_extent};
        m_pipeline.m_viewportState    = VkInit::GetPipelineViewportStateCreateInfo(viewport, scissor);
        m_pipeline.m_colorBlendState  = VkInit::GetPipelineColorBlendStateCreateInfo(m_flattenAttachmentBlendStates);
        m_pipeline.m_dynamicStates    = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        m_pipeline.m_depthStencilState =
            VkInit::GetPipelineDepthStencilStateCreateInfo(m_enableDepthStencil, m_enableDepthStencil);
        // descriptors
        m_pipeline.m_descriptorSetLayout.push_back(ViewUniform::GetDescriptorSetLayout());
    }

    void ForwardShadingPass::Run(RenderPassRunData run_data, ForwardShadingPassData pass_data)
    {
        // unpack data
        auto&& [frame_idx, cmd]                           = run_data;
        auto&& [viewport_size, instances, mesh, material] = pass_data;

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

        // bind data and draw
        {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());

            // set dynamic viewport
            VkViewport viewport = VkInit::GetViewport(viewport_size);
            VkRect2D   scissor  = {{0, 0}, viewport_size};
            vkCmdSetViewport(cmd, 0, 1, &viewport);
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            // bind view uniform
            auto&& view_uniform_descriptor =
                m_resourceManager.GetPerFramePersistantDescriptorSet(frame_idx, "ViewUniform/MainCamera");
            vkCmdBindDescriptorSets(
                cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipelineLayout(), 0, 1, &view_uniform_descriptor, 0, nullptr);

            // run renderpass
            auto&& layout = m_pipeline.GetPipelineLayout();
            {
                MaterialRenderPassData data = {cmd, layout};
                material->SetupShadingPass(data);
            }
            {
                MeshRenderPassData data = {frame_idx, cmd, layout, instances, shared_from_this()};
                mesh->RunRenderPass(data);
            }
        }

        // end render pass
        {
            vkCmdEndRenderPass(cmd);
        }
    }

    void CombinedForwardShadingPass::Resize(uint width, uint height)
    {
        for (auto&& [key, pass] : m_passes)
        {
            pass->Resize(width, height);
        }
        m_size = {width, height};
    }

    void CombinedForwardShadingPass::Run(RenderPassRunData run_data, CombinedForwardShadingPassData pass_data)
    {
        auto&& [frame_idx, cmd] = run_data;
        auto&& [renderables]    = pass_data;

        uint                     num_dispatched            = 0;
        uint                     total_dispatch            = renderables.size();
        for (auto&& [k, instances] : renderables)
        {
            num_dispatched++;
            auto mesh     = instances[0]->GetComponent<MeshComponent>().GetCoreValue();
            auto material = std::dynamic_pointer_cast<ForwardMaterial>(
                instances[0]->GetComponent<MaterialComponent>().GetCoreValue());

            // create pass if needed
            if (!StdUtils::Exists(m_passes, k))
            {
                auto&& color_targets = m_resourceManager.GetFramewiseImage("ColorRT");
                auto&& depth_targets = m_resourceManager.GetFramewiseImage("DepthRT");
                m_passes[k]          = std::make_shared<ForwardShadingPass>(
                    mesh, material, color_targets, depth_targets, m_resourceManager);
                m_passes[k]->Init(m_framesParallel);
                m_passes[k]->Resize(m_size.width, m_size.height);
            }

            /* ------------------------ dispatch pass ----------------------- */
            auto&&                 pass      = m_passes[k];
            ForwardShadingPassData pass_data = {
                m_size,
                instances,
                mesh,
                std::dynamic_pointer_cast<ForwardMaterial>(material),
            };
            pass->Run(run_data, pass_data);
        }
    }
} // namespace GE