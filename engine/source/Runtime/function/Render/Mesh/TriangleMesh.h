#pragma once

#include "GE_pch.h"

#include "../VulkanManager/GpuBuffer.h"
#include "../VulkanManager/VulkanCore.h"

#include "Runtime/core/IntervalJob.h"
#include "Runtime/core/Math/Math.h"

#include "Mesh.h"

namespace GE
{

    struct Vertex
    {
        float3 position = {0.0f, 0.0f, 0.0f};
        float3 normal   = {0.0f, 0.0f, 0.0f};
        float3 tangent  = {0.0f, 0.0f, 0.0f};
        float2 uv0      = {0.0f, 0.0f};

        static inline VertexInputDescription GetVertexInputDesc()
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

            description.attributes.push_back(positionAttribute);
            description.attributes.push_back(normalAttribute);
            description.attributes.push_back(tangentAttribute);
            description.attributes.push_back(texcoordAttribute);
            return description;
        }
    };

    class TriangleMesh : public Mesh
    {
    public:
        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;

        inline uint32_t GetTriangleCount() const { return m_indices.size() / 3; }
        inline uint32_t GetVertexCount() const { return m_vertices.size(); }
        inline uint32_t GetIndexCount() const { return m_indices.size(); }

        inline VkBuffer GetVertexBuffer() { return m_vertexBuffer.GetBuffer(); }
        inline VkBuffer GetIndexBuffer() { return m_indexBuffer.GetBuffer(); }

        void SetupPipeline(GraphicsRenderPipeline& pipeline) override;
        void RunRenderPass(MeshRenderPassData data) override;

        void Activate();
        void Deactivate();
        void Clear();

    private:
        bool Update();

    private:
        const Time::Miliseconds c_updateInterval = Time::Miliseconds(2000);
        const Time::Miliseconds c_deactiveThres  = Time::Miliseconds(5000);
        Time::TimeStamp         m_tLastUsed;
        IntervalJob             m_updateJob;

        bool      m_uploaded = false;
        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;
    };
} // namespace GE