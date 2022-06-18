#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"

namespace GE
{
    namespace VkInit
    {
        inline VkPipelineShaderStageCreateInfo
        GetPipelineShaderStageCreateInfo(VkShaderStageFlagBits            stage,
                                         VkShaderModule                   shaderModule,
                                         const char*                      pName = "main",
                                         VkPipelineShaderStageCreateFlags flags = 0)
        {
            VkPipelineShaderStageCreateInfo info = {};

            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = flags;

            info.stage  = stage;
            info.module = shaderModule;
            info.pName  = pName;

            return info;
        }

        inline VkPipelineVertexInputStateCreateInfo
        GetPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>&   bindings,
                                              const std::vector<VkVertexInputAttributeDescription>& attributes,
                                              VkPipelineVertexInputStateCreateFlags                 flags = 0)
        {
            VkPipelineVertexInputStateCreateInfo info = {};

            info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            info.pNext                           = nullptr;
            info.flags                           = flags;
            info.vertexBindingDescriptionCount   = bindings.size();
            info.pVertexBindingDescriptions      = bindings.data();
            info.vertexAttributeDescriptionCount = attributes.size();
            info.pVertexAttributeDescriptions    = attributes.data();

            return info;
        }

        /**
         * @brief initialize VkPipelineInputAssemblyStateCreateInfo object (defaults to triangle list)
         *
         * @param topology
         * @param primitiveRestartEnable
         * @param flags
         * @return VkPipelineInputAssemblyStateCreateInfo
         */
        inline VkPipelineInputAssemblyStateCreateInfo
        GetPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                bool                primitiveRestartEnable    = false,
                                                VkPipelineInputAssemblyStateCreateFlags flags = 0)
        {
            VkPipelineInputAssemblyStateCreateInfo info = {};

            info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            info.pNext                  = nullptr;
            info.flags                  = flags;
            info.topology               = topology;
            info.primitiveRestartEnable = primitiveRestartEnable;

            return info;
        }

        /**
         * @brief initialize VkPipelineRasterizationStateCreateInfo object
         *
         * @param polygonMode
         * @param cullMode
         * @param frontFace
         * @param depthClampEnable
         * @param rasterizerDiscardEnable
         * @param depthBiasConstantFactor
         * @param depthBiasClamp
         * @param depthBiasSlopeFactor
         * @param lineWidth
         * @param flags
         * @return VkPipelineRasterizationStateCreateInfo
         */
        inline VkPipelineRasterizationStateCreateInfo
        GetPipelineRasterizationStateCreateInfo(VkPolygonMode   polygonMode      = VK_POLYGON_MODE_FILL,
                                                VkCullModeFlags cullMode         = VK_CULL_MODE_NONE,
                                                VkFrontFace     frontFace        = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                bool            depthClampEnable = false,
                                                bool            rasterizerDiscardEnable       = false,
                                                float           depthBiasConstantFactor       = 0.0f,
                                                float           depthBiasClamp                = 0.0f,
                                                float           depthBiasSlopeFactor          = 0.0f,
                                                float           lineWidth                     = 1.0f,
                                                VkPipelineRasterizationStateCreateFlags flags = 0)
        {
            VkPipelineRasterizationStateCreateInfo info = {};

            info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            info.pNext                   = nullptr;
            info.flags                   = flags;
            info.depthClampEnable        = depthClampEnable ? VK_TRUE : VK_FALSE;
            info.rasterizerDiscardEnable = rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;
            info.polygonMode             = polygonMode;
            info.cullMode                = cullMode;
            info.frontFace               = frontFace;
            info.depthBiasEnable         = VK_FALSE;
            info.depthBiasConstantFactor = depthBiasConstantFactor;
            info.depthBiasClamp          = depthBiasClamp;
            info.depthBiasSlopeFactor    = depthBiasSlopeFactor;
            info.lineWidth               = lineWidth;

            return info;
        }

        /**
         * @brief initialize VkPipelineMultisampleStateCreateInfo object (defaults to one sample)
         *
         * @param rasterizationSamples
         * @param sampleShadingEnable
         * @param minSampleShading
         * @param flags
         * @return VkPipelineMultisampleStateCreateInfo
         */
        inline VkPipelineMultisampleStateCreateInfo
        GetPipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
                                              bool                  sampleShadingEnable   = false,
                                              float                 minSampleShading      = 1.0f,
                                              VkPipelineMultisampleStateCreateFlags flags = 0)
        {
            VkPipelineMultisampleStateCreateInfo info = {};

            info.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            info.pNext                 = nullptr;
            info.flags                 = flags;
            info.rasterizationSamples  = rasterizationSamples;
            info.sampleShadingEnable   = sampleShadingEnable ? VK_TRUE : VK_FALSE;
            info.minSampleShading      = minSampleShading;
            info.pSampleMask           = nullptr;
            info.alphaToCoverageEnable = VK_FALSE;
            info.alphaToOneEnable      = VK_FALSE;

            return info;
        }

        /**
         * @brief initialize VkPipelineColorBlendAttachmentState object (defaults to normal)
         *
         * @param blendEnable
         * @param srcColorBlendFactor
         * @param dstColorBlendFactor
         * @param colorBlendOp
         * @param srcAlphaBlendFactor
         * @param dstAlphaBlendFactor
         * @param alphaBlendOp
         * @param colorWriteMask
         * @return VkPipelineColorBlendAttachmentState
         */
        inline VkPipelineColorBlendAttachmentState GetPipelineColorBlendAttachmentState(
            bool                  blendEnable         = true,
            VkBlendFactor         srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            VkBlendFactor         dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VkBlendOp             colorBlendOp        = VK_BLEND_OP_ADD,
            VkBlendFactor         srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            VkBlendFactor         dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VkBlendOp             alphaBlendOp        = VK_BLEND_OP_ADD,
            VkColorComponentFlags colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
        {
            VkPipelineColorBlendAttachmentState info = {};

            info.blendEnable         = blendEnable ? VK_TRUE : VK_FALSE;
            info.srcColorBlendFactor = srcColorBlendFactor;
            info.dstColorBlendFactor = dstColorBlendFactor;
            info.colorBlendOp        = colorBlendOp;
            info.srcAlphaBlendFactor = srcAlphaBlendFactor;
            info.dstAlphaBlendFactor = dstAlphaBlendFactor;
            info.alphaBlendOp        = alphaBlendOp;
            info.colorWriteMask      = colorWriteMask;

            return info;
        }

        inline VkPipelineLayoutCreateInfo
        GetPipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                                    const std::vector<VkPushConstantRange>&   pushConstantRanges,
                                    VkPipelineLayoutCreateFlags               flags = 0)
        {
            VkPipelineLayoutCreateInfo info = {};

            info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext                  = nullptr;
            info.flags                  = flags;
            info.setLayoutCount         = descriptorSetLayouts.size();
            info.pSetLayouts            = descriptorSetLayouts.data();
            info.pushConstantRangeCount = pushConstantRanges.size();
            info.pPushConstantRanges    = pushConstantRanges.data();

            return info;
        }

        inline VkRenderPassCreateInfo GetRenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachments,
                                                              const std::vector<VkSubpassDescription>&    subpasses,
                                                              const std::vector<VkSubpassDependency>&     dependencies,
                                                              VkRenderPassCreateFlags                     flags = 0)
        {
            VkRenderPassCreateInfo info = {};

            info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            info.pNext           = nullptr;
            info.flags           = flags;
            info.attachmentCount = attachments.size();
            info.pAttachments    = attachments.data();
            info.subpassCount    = subpasses.size();
            info.pSubpasses      = subpasses.data();
            info.dependencyCount = dependencies.size();
            info.pDependencies   = dependencies.data();

            return info;
        }

        inline VkGraphicsPipelineCreateInfo
        GetGraphicsPipelineCreateInfo(VkPipelineLayout                                    pipelineLayout,
                                      VkRenderPass                                        renderPass,
                                      const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
                                      const VkPipelineVertexInputStateCreateInfo*         vertexInputState   = nullptr,
                                      const VkPipelineInputAssemblyStateCreateInfo*       inputAssemblyState = nullptr,
                                      const VkPipelineTessellationStateCreateInfo*        tessellationState  = nullptr,
                                      const VkPipelineViewportStateCreateInfo*            viewportState      = nullptr,
                                      const VkPipelineRasterizationStateCreateInfo*       rasterizationState = nullptr,
                                      const VkPipelineMultisampleStateCreateInfo*         multisampleState   = nullptr,
                                      const VkPipelineDepthStencilStateCreateInfo*        depthStencilState  = nullptr,
                                      const VkPipelineColorBlendStateCreateInfo*          colorBlendState    = nullptr,
                                      const VkPipelineDynamicStateCreateInfo*             dynamicState       = nullptr,
                                      VkPipeline            basePipelineHandle = VK_NULL_HANDLE,
                                      int32_t               basePipelineIndex  = 0,
                                      VkPipelineCreateFlags flags              = 0)
        {
            VkGraphicsPipelineCreateInfo info = {};

            info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = flags;

            info.stageCount = shaderStages.size();
            info.pStages    = shaderStages.data();

            info.pVertexInputState   = vertexInputState;
            info.pInputAssemblyState = inputAssemblyState;
            info.pTessellationState  = tessellationState;
            info.pViewportState      = viewportState;
            info.pRasterizationState = rasterizationState;
            info.pMultisampleState   = multisampleState;
            info.pDepthStencilState  = depthStencilState;
            info.pColorBlendState    = colorBlendState;
            info.pDynamicState       = dynamicState;

            info.layout = pipelineLayout;

            info.renderPass = renderPass;
            info.subpass    = 0;

            info.basePipelineHandle = basePipelineHandle;
            info.basePipelineIndex  = basePipelineIndex;

            return info;
        }
    } // namespace VkInit
} // namespace GE