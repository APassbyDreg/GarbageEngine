#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"

namespace GE
{
    namespace VkInit
    {
        inline VkFramebufferCreateInfo GetFramebufferCreateInfo(VkRenderPass                    renderpass,
                                                                VkExtent2D                      size,
                                                                const std::vector<VkImageView>& attachments,
                                                                uint                            layers = 1,
                                                                VkFramebufferCreateFlags        flags  = 0)

        {
            VkFramebufferCreateInfo info = {};
            info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.renderPass              = renderpass;
            info.attachmentCount         = static_cast<uint32_t>(attachments.size());
            info.pAttachments            = attachments.data();
            info.width                   = size.width;
            info.height                  = size.height;
            info.layers                  = layers;
            info.flags                   = flags;
            return info;
        }

        inline VkBufferCreateInfo GetBufferCreateInfo(VkDeviceSize       size,
                                                      VkBufferUsageFlags usage,
                                                      VkSharingMode      sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
                                                      VkBufferCreateFlags flags = 0)
        {
            VkBufferCreateInfo info = {};
            info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            info.size               = size;
            info.usage              = usage;
            info.sharingMode        = sharing_mode;
            info.flags              = flags;
            return info;
        }

        inline VmaAllocationCreateInfo GetAllocationCreateInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags = 0)
        {
            VmaAllocationCreateInfo info = {};
            info.usage                   = usage;
            info.flags                   = flags;
            return info;
        }

        inline VkDescriptorSetAllocateInfo
        GetDescriptorSetAllocateInfo(const std::vector<VkDescriptorSetLayout>& layouts,
                                     VkDescriptorPool                          pool = VK_NULL_HANDLE)
        {
            VkDescriptorSetAllocateInfo info = {};
            info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            info.descriptorPool              = pool;
            info.descriptorSetCount          = layouts.size();
            info.pSetLayouts                 = layouts.data();
            return info;
        }

        inline VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding(VkDescriptorType   type,
                                                                          VkShaderStageFlags stage_flags,
                                                                          uint32_t           binding,
                                                                          uint32_t           count = 1)
        {
            VkDescriptorSetLayoutBinding layout_binding = {};
            layout_binding.descriptorType               = type;
            layout_binding.stageFlags                   = stage_flags;
            layout_binding.binding                      = binding;
            layout_binding.descriptorCount              = count;
            return layout_binding;
        }

        inline VkDescriptorSetLayoutCreateInfo
        GetDescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                         VkDescriptorSetLayoutCreateFlags                 flags = 0)
        {
            VkDescriptorSetLayoutCreateInfo info = {};
            info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            info.bindingCount                    = bindings.size();
            info.pBindings                       = bindings.data();
            info.flags                           = flags;
            return info;
        }

        inline VkCommandPoolCreateInfo GetCommandPoolCreateInfo(uint32_t                 queue_family_index,
                                                                VkCommandPoolCreateFlags flags = 0)
        {
            VkCommandPoolCreateInfo info = {};
            info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.queueFamilyIndex        = queue_family_index;
            info.flags                   = flags;
            return info;
        }

        inline VkCommandBufferAllocateInfo
        GetCommandBufferAllocateInfo(VkCommandPool        cmd_pool,
                                     VkCommandBufferLevel level      = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                     uint32_t             num_buffer = 1)
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool                 = cmd_pool;
            info.level                       = level;
            info.commandBufferCount          = num_buffer;
            return info;
        }

        inline VkFenceCreateInfo GetFenceCreateInfo(VkFenceCreateFlags flags = 0)
        {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags             = flags;
            return fenceCreateInfo;
        }

        inline VkSemaphoreCreateInfo GetSemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0)
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreCreateInfo.flags                 = flags;
            return semaphoreCreateInfo;
        }

        inline VkPushConstantRange GetPushConstantRange(VkShaderStageFlags stage_flags,
                                                        uint32_t           size,
                                                        uint32_t           offset = 0)
        {
            VkPushConstantRange push_constant_range = {};
            push_constant_range.stageFlags          = stage_flags;
            push_constant_range.size                = size;
            push_constant_range.offset              = offset;
            return push_constant_range;
        }

        inline VkAttachmentDescription
        GetAttachmentDescription(VkFormat              format         = VK_FORMAT_R8G8B8A8_UNORM,
                                 VkSampleCountFlagBits samples        = VK_SAMPLE_COUNT_1_BIT,
                                 VkAttachmentLoadOp    loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD,
                                 VkAttachmentStoreOp   storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                                 VkAttachmentLoadOp    stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD,
                                 VkAttachmentStoreOp   stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                                 VkImageLayout         initialLayout  = VK_IMAGE_LAYOUT_GENERAL,
                                 VkImageLayout         finalLayout    = VK_IMAGE_LAYOUT_GENERAL)
        {
            VkAttachmentDescription attachment = {};
            attachment.format                  = format;
            attachment.samples                 = samples;
            attachment.loadOp                  = loadOp;
            attachment.storeOp                 = storeOp;
            attachment.stencilLoadOp           = stencilLoadOp;
            attachment.stencilStoreOp          = stencilStoreOp;
            attachment.initialLayout           = initialLayout;
            attachment.finalLayout             = finalLayout;
            return attachment;
        }

        inline VkViewport GetViewport(VkExtent2D extent)
        {
            VkViewport viewport = {};
            viewport.x          = 0.0f;
            viewport.y          = 0.0f;
            viewport.width      = (float)extent.width;
            viewport.height     = (float)extent.height;
            viewport.minDepth   = 0.0f;
            viewport.maxDepth   = 1.0f;
            return viewport;
        }

        inline VkPipelineViewportStateCreateInfo GetPipelineViewportStateCreateInfo(VkViewport viewport,
                                                                                    VkRect2D   scissor)
        {
            VkPipelineViewportStateCreateInfo info = {};
            info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            info.viewportCount                     = 1;
            info.pViewports                        = &viewport;
            info.scissorCount                      = 1;
            info.pScissors                         = &scissor;
            return info;
        }

        inline VkImageCreateInfo GetVkImageCreateInfo(VkImageType           type,
                                                      VkFormat              format,
                                                      VkExtent3D            extent,
                                                      VkImageUsageFlags     usage,
                                                      uint                  numMips     = 1,
                                                      uint                  arrayLayers = 1,
                                                      VkSampleCountFlagBits samples     = VK_SAMPLE_COUNT_1_BIT,
                                                      VkImageLayout         layout      = VK_IMAGE_LAYOUT_UNDEFINED,
                                                      VkImageTiling         tiling      = VK_IMAGE_TILING_OPTIMAL,
                                                      VkSharingMode         mode        = VK_SHARING_MODE_EXCLUSIVE,
                                                      VkFlags               flags       = 0)
        {
            VkImageCreateInfo info = {};
            info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType         = type;
            info.format            = format;
            info.extent            = extent;
            info.usage             = usage;
            info.mipLevels         = numMips;
            info.arrayLayers       = arrayLayers;
            info.samples           = samples;
            info.initialLayout     = layout;
            info.tiling            = tiling;
            info.sharingMode       = mode;
            info.flags             = flags;
            return info;
        }

        inline VkImageViewCreateInfo GetVkImageViewCreateInfo(VkImageCreateInfo  img_info    = {},
                                                              VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_NONE,
                                                              VkImageViewType    viewtype = VK_IMAGE_VIEW_TYPE_MAX_ENUM,
                                                              VkImage            image    = VK_NULL_HANDLE,
                                                              VkFlags            flags    = 0)
        {
            if (viewtype == VK_IMAGE_VIEW_TYPE_MAX_ENUM)
            {
                viewtype = (VkImageViewType)img_info.imageType;
            }

            VkImageViewCreateInfo info           = {};
            info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.pNext                           = nullptr;
            info.flags                           = 0;
            info.image                           = image;
            info.viewType                        = viewtype;
            info.format                          = img_info.format;
            info.components.r                    = VK_COMPONENT_SWIZZLE_R;
            info.components.g                    = VK_COMPONENT_SWIZZLE_G;
            info.components.b                    = VK_COMPONENT_SWIZZLE_B;
            info.components.a                    = VK_COMPONENT_SWIZZLE_A;
            info.subresourceRange.aspectMask     = aspectFlags;
            info.subresourceRange.baseMipLevel   = 0;
            info.subresourceRange.levelCount     = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount     = 1;
            return info;
        }

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
            bool                  blendEnable         = false,
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

        inline VkPipelineColorBlendStateCreateInfo
        GetPipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
                                             bool                                 logicOpEnable = false,
                                             VkLogicOp                            logicOp       = VK_LOGIC_OP_COPY,
                                             VkPipelineColorBlendStateCreateFlags flags         = 0)
        {
            VkPipelineColorBlendStateCreateInfo info = {};

            info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            info.pNext             = nullptr;
            info.flags             = flags;
            info.logicOpEnable     = logicOpEnable ? VK_TRUE : VK_FALSE;
            info.logicOp           = logicOp;
            info.attachmentCount   = attachments.size();
            info.pAttachments      = attachments.data();
            info.blendConstants[0] = 0.0f;
            info.blendConstants[1] = 0.0f;
            info.blendConstants[2] = 0.0f;
            info.blendConstants[3] = 0.0f;

            return info;
        }

        inline VkPipelineDynamicStateCreateInfo
        GetPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates,
                                          VkPipelineDynamicStateCreateFlags  flags = 0)
        {
            VkPipelineDynamicStateCreateInfo info = {};

            info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            info.pNext             = nullptr;
            info.flags             = flags;
            info.dynamicStateCount = dynamicStates.size();
            info.pDynamicStates    = dynamicStates.data();
            return info;
        }

        inline VkPipelineDepthStencilStateCreateInfo
        GetPipelineDepthStencilStateCreateInfo(bool             depthTest   = false,
                                               bool             depthWrite  = false,
                                               VkCompareOp      depthOp     = VK_COMPARE_OP_LESS_OR_EQUAL,
                                               float2           depthBounds = float2(0, 1),
                                               bool             stencilTest = false,
                                               VkStencilOpState front       = {},
                                               VkStencilOpState back        = {})
        {
            bool depthBoundTest = depthBounds.x < depthBounds.y;

            VkPipelineDepthStencilStateCreateInfo info = {};
            info.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            info.pNext                                 = nullptr;

            info.depthTestEnable  = depthTest ? VK_TRUE : VK_FALSE;
            info.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE;
            info.depthCompareOp   = depthTest ? depthOp : VK_COMPARE_OP_ALWAYS;

            // info.depthBoundsTestEnable = depthBoundTest ? VK_TRUE : VK_FALSE;
            info.depthBoundsTestEnable = VK_FALSE; // REVIEW: enable depth bound test
            info.minDepthBounds        = depthBounds.x;
            info.maxDepthBounds        = depthBounds.y;

            info.stencilTestEnable = stencilTest ? VK_TRUE : VK_FALSE;
            info.front             = front;
            info.back              = back;

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

        inline VkComputePipelineCreateInfo GetComputePipelineCreateInfo(VkPipelineLayout                pipelineLayout,
                                                                        VkPipelineShaderStageCreateInfo shaderStage,
                                                                        VkPipeline basePipelineHandle = VK_NULL_HANDLE,
                                                                        int32_t    basePipelineIndex  = -1)
        {
            VkComputePipelineCreateInfo info = {};

            info.sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            info.pNext              = nullptr;
            info.flags              = 0;
            info.stage              = shaderStage;
            info.layout             = pipelineLayout;
            info.basePipelineHandle = basePipelineHandle;
            info.basePipelineIndex  = basePipelineIndex;

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