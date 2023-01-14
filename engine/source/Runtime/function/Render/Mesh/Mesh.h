#pragma once

#include "GE_pch.h"

#include "../RenderPipeline.h"
#include "../VulkanManager/VulkanCore.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct MeshRenderPassData
    {
        VkCommandBuffer  cmd;
        VkPipelineLayout layout;
        uint num_instance;
    };

    class Mesh
    {
    public:
        virtual void SetupPipeline(GraphicsRenderPipeline& pipeline) = 0;
        virtual void RunRenderPass(MeshRenderPassData data)          = 0;
    };
} // namespace GE