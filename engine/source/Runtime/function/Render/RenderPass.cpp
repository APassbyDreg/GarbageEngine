#include "RenderPass.h"

#include "VulkanManager/VulkanCore.h"
#include "VulkanManager/VulkanCreateInfoBuilder.h"

namespace GE
{
    GraphicsPass::~GraphicsPass()
    {
        if (m_ready)
        {
            vkDestroyRenderPass(VulkanCore::GetDevice(), m_renderPass, nullptr);
            m_ready = false;
        }
    }

    void GraphicsPass::BuildInternal()
    {
        if (m_ready)
        {
            GE_CORE_WARN("GraphicsPass [{}] is built repeatedly!", m_name);
            return;
        }

        // __update_resource();
        // we assume the resources is already in place

        {
            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.inputAttachmentCount = m_inputRefference.size();
            subpass.pInputAttachments    = m_inputRefference.data();
            subpass.colorAttachmentCount = m_outputReference.size();
            subpass.pColorAttachments    = m_outputReference.data();
            if (m_enableDepthStencil)
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

            if (m_enableDepthStencil)
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

        {
            VkRenderPassCreateInfo info =
                VkInit::GetRenderPassCreateInfo(m_flattenAttachments, m_subpasses, m_dependencies);
            GE_VK_ASSERT(vkCreateRenderPass(VulkanCore::GetDevice(), &info, nullptr, &m_renderPass));
            m_ready = true;
        }

        m_pipeline.Build(m_renderPass);
    }

    void GraphicsPass::__update_resource()
    {
        m_flattenAttachments.clear();
        m_inputRefference.clear();
        m_outputReference.clear();
        /* ------- make references and flatten attachment descriptions ------ */
        int ref_idx = 0;
        // inputs
        for (int i = 0; i < m_input.size(); i++, ref_idx++)
        {
            VkAttachmentReference ref = {};
            ref.attachment            = ref_idx;
            ref.layout                = m_input[i].layout;
            m_flattenAttachments.push_back(m_input[i].desc);
            m_flattenAttachmentBlendStates.push_back(m_input[i].blend);
            m_inputRefference.push_back(ref);
        }
        // outputs
        for (int i = 0; i < m_output.size(); i++, ref_idx++)
        {
            VkAttachmentReference ref = {};
            ref.attachment            = ref_idx;
            ref.layout                = m_output[i].layout;
            m_flattenAttachments.push_back(m_output[i].desc);
            m_flattenAttachmentBlendStates.push_back(m_output[i].blend);
            m_outputReference.push_back(ref);
        }
        // depth / stencil
        if (m_enableDepthStencil)
        {
            m_depthReference.attachment = ref_idx;
            m_depthReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            m_flattenAttachments.push_back(m_depthAttachment);
        }
    }

    ComputePass::~ComputePass()
    {
        if (m_ready)
        {
            m_ready = false;
        }
    }

    void ComputePass::BuildInternal()
    {
        if (m_ready)
        {
            GE_CORE_WARN("ComputePass [{}] is built repeatedly!", m_name);
            return;
        }

        m_pipeline.Build();
    }

} // namespace GE