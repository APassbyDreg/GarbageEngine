#pragma once

#include "GE_pch.h"

#include "../VulkanManager/VulkanCore.h"

#include "Runtime/core/math/math.h"

namespace GE
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct Vertex
    {
        float3 position = {0.0f, 0.0f, 0.0f};
        float3 normal   = {0.0f, 0.0f, 0.0f};
        float3 tangent  = {0.0f, 0.0f, 0.0f};
        float2 uv0      = {0.0f, 0.0f};
        uint   flags    = 0;

        static VertexInputDescription GetVertexInputDesc()
        {
            VertexInputDescription description;

            // we will have just 1 vertex buffer binding, with a per-vertex rate
            VkVertexInputBindingDescription mainBinding = {};
            mainBinding.binding                         = 0;
            mainBinding.stride                          = sizeof(Vertex);
            mainBinding.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

            description.bindings.push_back(mainBinding);

            // Position
            VkVertexInputAttributeDescription positionAttribute = {};
            positionAttribute.binding                           = 0;
            positionAttribute.location                          = 0;
            positionAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
            positionAttribute.offset                            = offsetof(Vertex, position);

            // Normal
            VkVertexInputAttributeDescription normalAttribute = {};
            normalAttribute.binding                           = 0;
            normalAttribute.location                          = 1;
            normalAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
            normalAttribute.offset                            = offsetof(Vertex, normal);

            // Tangent
            VkVertexInputAttributeDescription tangentAttribute = {};
            tangentAttribute.binding                           = 0;
            tangentAttribute.location                          = 2;
            tangentAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
            tangentAttribute.offset                            = offsetof(Vertex, tangent);

            // UV0
            VkVertexInputAttributeDescription texcoordAttribute = {};
            texcoordAttribute.binding                           = 0;
            texcoordAttribute.location                          = 3;
            texcoordAttribute.format                            = VK_FORMAT_R32G32_SFLOAT;
            texcoordAttribute.offset                            = offsetof(Vertex, uv0);

            // flags
            VkVertexInputAttributeDescription flagsAttribute = {};
            flagsAttribute.binding                           = 0;
            flagsAttribute.location                          = 4;
            flagsAttribute.format                            = VK_FORMAT_R32_UINT;
            flagsAttribute.offset                            = offsetof(Vertex, flags);

            description.attributes.push_back(positionAttribute);
            description.attributes.push_back(normalAttribute);
            description.attributes.push_back(tangentAttribute);
            description.attributes.push_back(texcoordAttribute);
            description.attributes.push_back(flagsAttribute);
            return description;
        }
    };

    struct Mesh
    {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
    };
} // namespace GE