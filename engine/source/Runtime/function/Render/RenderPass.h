#pragma once

#include "GE_pch.h"

#include "Runtime/core/math/math.h"

#include "VulkanManager/ShaderManager.h"
#include "VulkanManager/VulkanCreateInfoBuilder.h"

#include "RenderPipeline.h"

namespace GE
{
    class RenderPassResource
    {
    public:
        RenderPassResource()
        {
            desc   = VkInit::GetAttachmentDescription();
            blend  = VkInit::GetPipelineColorBlendAttachmentState();
            layout = VK_IMAGE_LAYOUT_GENERAL;
        }
        VkAttachmentDescription             desc;
        VkPipelineColorBlendAttachmentState blend;
        VkImageLayout                       layout;
    };

    /**
     * @brief warper for vulkan render pass, defaults to have only one subpass
     */
    class RenderPass
    {
    public:
        ~RenderPass();

        virtual void Build();

        inline VkRenderPass     GetRenderPass() { return m_renderPass; }
        inline VkPipeline       GetPipeline() { return m_pipeline.GetPipeline(); }
        inline VkPipelineLayout GetPipelineLayout() { return m_pipeline.GetPipelineLayout(); }

    protected:
        void __update_resource();

    public:
        std::vector<RenderPassResource> m_input, m_output;

        std::string m_name;

    protected:
        VkRenderPass m_renderPass;

        bool m_ready = false;

        bool                    m_enableDepthStencil = false;
        VkAttachmentDescription m_depthAttachment;
        VkAttachmentReference   m_depthReference;

        std::vector<VkAttachmentDescription>             m_flattenAttachments;
        std::vector<VkPipelineColorBlendAttachmentState> m_flattenAttachmentBlendStates;
        std::vector<VkAttachmentReference>               m_inputRefference, m_outputReference;
        std::vector<VkSubpassDependency>                 m_dependencies;
        std::vector<VkSubpassDescription>                m_subpasses;

        GraphicsRenderPipeline m_pipeline;

        VkExtent2D m_extent = {1920, 1080};
    };
} // namespace GE