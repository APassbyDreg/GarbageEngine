#include "RenderPipeline.h"

#include "VulkanManager/VulkanCreateInfoBuilder.h"

#include "RenderPass.h"

namespace GE
{
    GraphicsRenderPipeline::~GraphicsRenderPipeline()
    {
        vkDestroyPipelineLayout(VulkanCore::GetVkDevice(), m_pipelineLayout, nullptr);
        vkDestroyPipeline(VulkanCore::GetVkDevice(), m_pipeline, nullptr);
        m_ready = false;
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
        VK_CHECK(vkCreatePipelineLayout(VulkanCore::GetVkDevice(), &layout_info, nullptr, &m_pipelineLayout));

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
                                                          nullptr);
        VK_CHECK(vkCreateGraphicsPipelines(VulkanCore::GetVkDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &m_pipeline));

        m_ready = true;
    }
} // namespace GE