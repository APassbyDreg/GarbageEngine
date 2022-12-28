#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "ShaderManager/ShaderModule.h"
#include "VulkanManager/VulkanCore.h"

namespace GE
{
    class GraphicsRenderPipeline
    {
    public:
        ~GraphicsRenderPipeline();

        void Build(VkRenderPass pass, VkPipelineCache cache = VK_NULL_HANDLE);

        inline bool       IsReady() { return m_ready; }
        
        inline VkPipeline GetPipeline()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipeline: Pipeline is not ready!");
            return m_pipeline;
        }

        inline VkPipelineLayout GetPipelineLayout()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipelineLayout: Pipeline is not ready!");
            return m_pipelineLayout;
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

    class ComputeRenderPipeline
    {
    public:
        ~ComputeRenderPipeline();

        void Build(VkRenderPass pass, VkPipelineCache cache = VK_NULL_HANDLE);

        inline bool IsReady() { return m_ready; }

        inline VkPipeline GetPipeline()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipeline: Pipeline is not ready!");
            return m_pipeline;
        }

        inline VkPipelineLayout GetPipelineLayout()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipelineLayout: Pipeline is not ready!");
            return m_pipelineLayout;
        }

    public:
        std::shared_ptr<ShaderModule>      m_shader;
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayout;
        std::vector<VkPushConstantRange>   m_pushConstantRanges;

    private:
        bool m_ready = false;

        VkPipeline       m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
} // namespace GE