#include "RenderPipeline.h"

#include "VulkanManager/DescriptorSetLayout.h"
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

    void GraphicsRenderPipeline::AddPushConstant(std::string identifier, VkShaderStageFlagBits stage, uint size)
    {
        uint                               stage_bit = 0;
        std::vector<VkShaderStageFlagBits> stages    = {};
        std::vector<uint>                  offsets   = {};
        for (uint i = 0; i < 32; i++)
        {
            VkShaderStageFlagBits bit = (VkShaderStageFlagBits)(1 << i);
            if (stage & bit)
            {
                stages.push_back(bit);
                offsets.push_back(m_pushConstantsSizes[bit]);
                m_pushConstantsSizes[bit] += size;
                m_pushConstantsLayouts[bit].push_back(size);
            }
        }
        m_pushConstantFns[identifier] = [=](VkCommandBuffer cmd, void* data) {
            for (uint i = 0; i < stages.size(); i++)
            {
                vkCmdPushConstants(cmd, m_pipelineLayout, stages[i], offsets[i], size, data);
            }
        };
    }

    void GraphicsRenderPipeline::PushConstant(std::string identifier, VkCommandBuffer cmd, void* data)
    {
        m_pushConstantFns[identifier](cmd, data);
    }

    void GraphicsRenderPipeline::Build(VkRenderPass pass, VkPipelineCache cache)
    {
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {};
        for (auto& shader : m_shaders)
        {
            shader_stages.push_back(shader->GetShaderStage());
        }

        // layout
        for (uint i = 0; i < m_descriptorSetLayoutBindings.size(); i++)
        {
            if (m_descriptorSetLayout.size() <= i)
            {
                m_descriptorSetLayout.push_back(VK_NULL_HANDLE);
            }
            if (m_descriptorSetLayout[i] != VK_NULL_HANDLE || m_descriptorSetLayoutBindings[i].empty())
            {
                continue;
            }
            auto info                = VkInit::GetDescriptorSetLayoutCreateInfo(m_descriptorSetLayoutBindings[i]);
            auto layout              = VulkanCore::CreateDescriptorSetLayout(info);
            m_descriptorSetLayout[i] = layout;
        }
        for (int i = 0; i < m_descriptorSetLayout.size(); i++)
        {
            if (m_descriptorSetLayout[i] == VK_NULL_HANDLE)
            {
                m_descriptorSetLayout[i] = GetEmptyDescriptorSetLayout();
            }
        }
        for (auto& [stage, sizes] : m_pushConstantsLayouts)
        {
            uint offset = 0;
            for (auto& size : sizes)
            {
                m_pushConstantRanges.push_back(VkInit::GetPushConstantRange(stage, size, offset));
                offset += size;
            }
        }
        auto layout_info = VkInit::GetPipelineLayoutCreateInfo(m_descriptorSetLayout, m_pushConstantRanges);
        GE_VK_ASSERT(vkCreatePipelineLayout(VulkanCore::GetDevice(), &layout_info, nullptr, &m_pipelineLayout));

        // dynamic states
        auto dynamic_states = VkInit::GetPipelineDynamicStateCreateInfo(m_dynamicStates);

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
                                                          &dynamic_states);
        GE_VK_ASSERT(
            vkCreateGraphicsPipelines(VulkanCore::GetDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &m_pipeline));

        m_ready = true;
    }

    VkDescriptorSetLayout GraphicsRenderPipeline::GetEmptyDescriptorSetLayout()
    {
        static DescriptorSetLayout layout;
        if (!layout.IsValid())
        {
            auto info = VkInit::GetDescriptorSetLayoutCreateInfo({});
            layout.Create(info);
        }
        return layout;
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

    void ComputeRenderPipeline::Build(VkPipelineCache cache)
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