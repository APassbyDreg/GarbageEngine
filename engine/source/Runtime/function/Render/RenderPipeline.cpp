#include "RenderPipeline.h"

#include "VulkanManager/VulkanCreateInfoBuilder.h"

#include "RenderPass.h"

namespace GE
{
    GraphicsRenderPipeline::~GraphicsRenderPipeline()
    {
        if (m_ready)
        {
            vkDestroyPipelineLayout(VulkanCore::GetDevice(), m_pipelineLayout, nullptr);
            vkDestroyPipeline(VulkanCore::GetDevice(), m_pipeline, nullptr);
            m_ready = false;
        }
    }

    void GraphicsRenderPipeline::Build(VkRenderPass pass, VkPipelineCache cache)
    {
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {};
        for (auto& shader : m_shaders)
        {
            shader_stages.push_back(shader->GetShaderStage());
        }

        // layout
        auto layout_info = VkInit::GetPipelineLayoutCreateInfo(m_descriptorSetLayout, m_pushConstantRanges);
        GE_VK_ASSERT(vkCreatePipelineLayout(VulkanCore::GetDevice(), &layout_info, nullptr, &m_pipelineLayout));

        // pipeline
        auto info = VkInit::GetGraphicsPipelineCreateInfo(m_pipelineLayout,
                                                          pass,
                                                          shader_stages,
                                                          &m_vertexInputState,
                                                          &m_inputAssemblyState,
                                                          nullptr,
                                                          &m_viewportState,
                                                          &m_rasterizationState,
                                                          &m_multisampleState,
                                                          &m_depthStencilState,
                                                          &m_colorBlendState,
                                                          &m_dynamicState);
        GE_VK_ASSERT(
            vkCreateGraphicsPipelines(VulkanCore::GetDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &m_pipeline));

        m_ready = true;
    }

    ComputeRenderPipeline::~ComputeRenderPipeline()
    {
        if (m_ready)
        {
            vkDestroyPipelineLayout(VulkanCore::GetDevice(), m_pipelineLayout, nullptr);
            vkDestroyPipeline(VulkanCore::GetDevice(), m_pipeline, nullptr);
            m_ready = false;
        }
    }

    void ComputeRenderPipeline::Build(VkRenderPass pass, VkPipelineCache cache)
    {
        // layout
        auto layout_info = VkInit::GetPipelineLayoutCreateInfo(m_descriptorSetLayout, m_pushConstantRanges);
        GE_VK_ASSERT(vkCreatePipelineLayout(VulkanCore::GetDevice(), &layout_info, nullptr, &m_pipelineLayout));

        // pipeline
        auto info = VkInit::GetComputePipelineCreateInfo(m_pipelineLayout, m_shader->GetShaderStage());

        GE_VK_ASSERT(
            vkCreateComputePipelines(VulkanCore::GetDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &m_pipeline));
    }
} // namespace GE