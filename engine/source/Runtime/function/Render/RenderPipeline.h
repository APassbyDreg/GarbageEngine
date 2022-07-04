#pragma once

#include "GE_pch.h"

#include "Runtime/core/math/math.h"

#include "VulkanManager/ShaderManager.h"
#include "VulkanManager/VulkanCore.h"

namespace GE
{
    class GraphicsRenderPipeline
    {
    public:
        ~GraphicsRenderPipeline();

        void Build(VkRenderPass pass, VkPipelineCache cache = VK_NULL_HANDLE);

        inline VkPipeline GetPipeline()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipeline: Pipeline is not ready!");
            return m_pipeline;
        }

    public:
        std::vector<std::shared_ptr<ShaderModule>> m_shaders;

        VkPipelineVertexInputStateCreateInfo   m_vertexInputState   = {};
        VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyState = {};
        VkPipelineTessellationStateCreateInfo  m_tessellationState  = {};
        VkPipelineViewportStateCreateInfo      m_viewportState      = {};
        VkPipelineRasterizationStateCreateInfo m_rasterizationState = {};
        VkPipelineMultisampleStateCreateInfo   m_multisampleState   = {};
        VkPipelineDepthStencilStateCreateInfo  m_depthStencilState  = {};
        VkPipelineColorBlendStateCreateInfo    m_colorBlendState    = {};
        VkPipelineDynamicStateCreateInfo       m_dynamicState       = {};

        std::vector<VkDescriptorSetLayout> m_descriptorSetLayout;
        std::vector<VkPushConstantRange>   m_pushConstantRanges;

    private:
        bool m_ready = false;

        VkPipeline       m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
} // namespace GE