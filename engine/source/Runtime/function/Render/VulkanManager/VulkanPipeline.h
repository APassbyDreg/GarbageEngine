#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"
#include "VulkanRenderPass.h"

namespace GE
{
    class VulkanPipeline
    {
    public:
        inline VkPipeline GetPipeline() { return m_pipeline; }

    private:
        /* ------------------------------ resources ----------------------------- */

        VulkanRenderPass                             m_renderPass;
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
        VkPipelineVertexInputStateCreateInfo         m_vertexInputState;
        VkPipelineInputAssemblyStateCreateInfo       m_inputAssemblyState;
        VkPipelineViewportStateCreateInfo            m_viewportState;
        VkPipelineRasterizationStateCreateInfo       m_rasterizationState;
        VkPipelineMultisampleStateCreateInfo         m_multisampleState;
        VkPipelineDepthStencilStateCreateInfo        m_depthStencilState;
        VkPipelineColorBlendStateCreateInfo          m_colorBlendState;
        VkPipelineDynamicStateCreateInfo             m_dynamicState;
        VkPipelineLayout                             m_pipelineLayout;

        /* ------------------------------ pipeline ----------------------------- */

        VkPipeline m_pipeline;
    };
} // namespace GE