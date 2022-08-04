#include "RenderPass.h"

#include "VulkanManager/VulkanCore.h"
#include "VulkanManager/VulkanCreateInfoBuilder.h"

namespace GE
{
    RenderPass::~RenderPass()
    {
        if (m_ready)
        {
            vkDestroyRenderPass(VulkanCore::GetVkDevice(), m_renderPass, nullptr);
            m_ready = false;
        }
    }

    void RenderPass::Build()
    {
        if (m_ready)
        {
            GE_CORE_WARN("RenderPass [{}] is built repeatedly!", m_name);
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
        }

        {
            VkRenderPassCreateInfo info =
                VkInit::GetRenderPassCreateInfo(m_flattenAttachments, m_subpasses, m_dependencies);
            VK_CHECK(vkCreateRenderPass(VulkanCore::GetVkDevice(), &info, nullptr, &m_renderPass));
            m_ready = true;
        }
    }

    void RenderPass::__update_resource()
    {
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

} // namespace GE