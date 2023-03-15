#include "RenderPass.h"

#include "VulkanManager/VulkanCore.h"
#include "VulkanManager/VulkanCreateInfoBuilder.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    void RenderPass::Build()
    {
        if (m_built)
        {
            return;
        }

        m_flattenAttachments.clear();
        m_inputRefference.clear();
        m_outputReference.clear();
        /* ------- make references and flatten attachment descriptions ------ */
        int ref_idx = 0;
        // inputs
        for (int i = 0; i < input.size(); i++, ref_idx++)
        {
            VkAttachmentReference ref = {};
            ref.attachment            = ref_idx;
            ref.layout                = input[i].layout;
            m_flattenAttachments.push_back(input[i].desc);
            m_flattenAttachmentBlendStates.push_back(input[i].blend);
            m_inputRefference.push_back(ref);
        }
        // outputs
        for (int i = 0; i < output.size(); i++, ref_idx++)
        {
            VkAttachmentReference ref = {};
            ref.attachment            = ref_idx;
            ref.layout                = output[i].layout;
            m_flattenAttachments.push_back(output[i].desc);
            m_flattenAttachmentBlendStates.push_back(output[i].blend);
            m_outputReference.push_back(ref);
        }
        // depth / stencil
        if (enableDepthStencil)
        {
            m_depthReference.attachment = ref_idx;
            m_depthReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            m_flattenAttachments.push_back(depthAttachment);
        }

        /* ------------------------- setup subpasses ------------------------ */
        {
            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.inputAttachmentCount = m_inputRefference.size();
            subpass.pInputAttachments    = m_inputRefference.data();
            subpass.colorAttachmentCount = m_outputReference.size();
            subpass.pColorAttachments    = m_outputReference.data();
            if (enableDepthStencil)
            {
                subpass.pDepthStencilAttachment = &m_depthReference;
            }
            m_subpasses.push_back(subpass);
        }

        {
            VkSubpassDependency dependency = {};
            dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass          = 0;
            dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask       = 0;
            dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            m_dependencies.push_back(dependency);

            if (enableDepthStencil)
            {
                VkSubpassDependency depth_dependency = {};
                depth_dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
                depth_dependency.dstSubpass          = 0;
                depth_dependency.srcStageMask =
                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                depth_dependency.srcAccessMask = 0;
                depth_dependency.dstStageMask =
                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                m_dependencies.push_back(depth_dependency);
            }
        }

        /* ------------------------------ build ----------------------------- */
        auto&& info = VkInit::GetRenderPassCreateInfo(m_flattenAttachments, m_subpasses, m_dependencies);
        vkCreateRenderPass(VulkanCore::GetDevice(), &info, nullptr, &m_renderPass);

        m_built = true;
    }

    GraphicsPassBase::~GraphicsPassBase() {}

    std::shared_ptr<GraphicsRenderPipeline> GraphicsPassBase::BuildPipeline()
    {
        auto pipeline = std::make_shared<GraphicsRenderPipeline>();
        for (auto&& fn : m_pipelineSetupFns)
        {
            fn(*pipeline);
        }
        pipeline->Build(GetRenderPass());
        return pipeline;
    }
} // namespace GE