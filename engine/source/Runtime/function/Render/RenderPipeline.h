#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "ShaderManager/ShaderModule.h"
#include "VulkanManager/DescriptorSetLayout.h"
#include "VulkanManager/VulkanCore.h"

namespace GE
{

    class GraphicsRenderPipeline
    {
    public:
        GraphicsRenderPipeline()
        {
            m_pushConstantsLayouts = {};
            m_pushConstantsSizes   = {};
            for (uint i = 0; i < 32; i++)
            {
                VkShaderStageFlagBits stage   = (VkShaderStageFlagBits)(1 << i);
                m_pushConstantsLayouts[stage] = {};
                m_pushConstantsSizes[stage]   = 0;
            }
        }
        ~GraphicsRenderPipeline();

        void Build(VkRenderPass pass, VkPipelineCache cache = VK_NULL_HANDLE);

        inline bool       IsReady() { return m_ready; }
        
        inline VkPipeline GetPipeline()
        {
            GE_CORE_CHECK(m_ready, "GraphicsRenderPipeline::GetPipeline: Trying to get an uninitialized pipeline");
            return m_pipeline;
        }

        inline VkPipelineLayout GetPipelineLayout()
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipelineLayout: Pipeline is not ready!");
            return m_pipelineLayout;
        }

        inline const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipelineLayouts: Pipeline is not ready!");
            return m_descriptorSetLayout;
        }

        inline VkDescriptorSetLayout GetDescriptorSetLayout(uint index) const
        {
            GE_CORE_ASSERT(m_ready, "GraphicsRenderPipeline::GetPipelineLayout: Pipeline is not ready!");
            return m_descriptorSetLayout[index];
        }

        inline void SetDescriptorSetLayout(uint set_id, VkDescriptorSetLayout layout)
        {
            while (m_descriptorSetLayout.size() <= set_id)
            {
                m_descriptorSetLayout.push_back(VK_NULL_HANDLE);
            }
            m_descriptorSetLayout[set_id] = layout;
        }

        inline void AddDescriptorSetLayoutBinding(uint set_id, VkDescriptorSetLayoutBinding binding)
        {
            while (m_descriptorSetLayoutBindings.size() <= set_id)
            {
                m_descriptorSetLayoutBindings.push_back({});
            }
            m_descriptorSetLayoutBindings[set_id].push_back(binding);
        }

        inline void AddDescriptorSetLayoutBinding(uint set_id, std::vector<VkDescriptorSetLayoutBinding> bindings)
        {
            while (m_descriptorSetLayoutBindings.size() <= set_id)
            {
                m_descriptorSetLayoutBindings.push_back({});
            }
            for (auto& b : bindings)
            {
                m_descriptorSetLayoutBindings[set_id].push_back(b);
            }
        }

        static VkDescriptorSetLayout GetEmptyDescriptorSetLayout();

        using PushConstantFn = std::function<void(VkCommandBuffer, void*)>;
        void AddPushConstant(std::string identifier, VkShaderStageFlagBits stage, uint size);
        void PushConstant(std::string identifier, VkCommandBuffer cmd, void* data);

        void AddShaderModule(std::unique_ptr<ShaderModule> shaderModule);

    public:
        VkPipelineVertexInputStateCreateInfo   m_vertexInputState   = {};
        VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyState = {};
        VkPipelineTessellationStateCreateInfo  m_tessellationState  = {};
        VkPipelineViewportStateCreateInfo      m_viewportState      = {};
        VkPipelineRasterizationStateCreateInfo m_rasterizationState = {};
        VkPipelineMultisampleStateCreateInfo   m_multisampleState   = {};
        VkPipelineDepthStencilStateCreateInfo  m_depthStencilState  = {};
        VkPipelineColorBlendStateCreateInfo    m_colorBlendState    = {};

        std::vector<VkDynamicState> m_dynamicStates;

    private:
        bool m_ready = false;

        std::map<VkShaderStageFlagBits, std::unique_ptr<ShaderModule>> m_shaders;

        std::map<VkShaderStageFlagBits, std::vector<uint>> m_pushConstantsLayouts;
        std::map<VkShaderStageFlagBits, uint>              m_pushConstantsSizes;
        std::map<std::string, PushConstantFn>              m_pushConstantFns;

        VkPipeline                                             m_pipeline;
        VkPipelineLayout                                       m_pipelineLayout;
        std::vector<VkDescriptorSetLayout>                     m_descriptorSetLayout;
        std::vector<VkPushConstantRange>                       m_pushConstantRanges;
        std::vector<std::vector<VkDescriptorSetLayoutBinding>> m_descriptorSetLayoutBindings;

        std::vector<DescriptorSetLayout> m_pipelineSpecificDescriptorSetLayouts;
    };

    class ComputeRenderPipeline
    {
    public:
        ~ComputeRenderPipeline();

        void Build(VkPipelineCache cache = VK_NULL_HANDLE);

        inline bool IsReady() { return m_ready; }

        inline VkPipeline GetPipeline()
        {
            GE_CORE_ASSERT(m_ready, "ComputeRenderPipeline::GetPipeline: Pipeline is not ready!");
            return m_pipeline;
        }

        inline VkPipelineLayout GetPipelineLayout()
        {
            GE_CORE_ASSERT(m_ready, "ComputeRenderPipeline::GetPipelineLayout: Pipeline is not ready!");
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

    class RenderPipelineManager : public Singleton<RenderPipelineManager>
    {
    public:
        static inline std::shared_ptr<GraphicsRenderPipeline> GetGraphicsPipeline(const std::string& name)
        {
            return GetInstance().m_graphicsPipelines[name];
        }

        static inline std::shared_ptr<ComputeRenderPipeline> GetComputePipeline(const std::string& name)
        {
            return GetInstance().m_computePipelines[name];
        }

        static inline void RegisterGraphicsPipeline(const std::string&                      name,
                                                    std::shared_ptr<GraphicsRenderPipeline> pipeline)
        {
            GetInstance().m_graphicsPipelines[name] = pipeline;
        }

        static inline void RegisterComputePipeline(const std::string&                     name,
                                                   std::shared_ptr<ComputeRenderPipeline> pipeline)
        {
            GetInstance().m_computePipelines[name] = pipeline;
        }

        static inline bool HasGraphicsPipeline(const std::string& name)
        {
            return GetInstance().m_graphicsPipelines.find(name) != GetInstance().m_graphicsPipelines.end();
        }

        static inline bool HasComputePipeline(const std::string& name)
        {
            return GetInstance().m_computePipelines.find(name) != GetInstance().m_computePipelines.end();
        }

    private:
        std::map<std::string, std::shared_ptr<GraphicsRenderPipeline>> m_graphicsPipelines;
        std::map<std::string, std::shared_ptr<ComputeRenderPipeline>>  m_computePipelines;
    };
} // namespace GE