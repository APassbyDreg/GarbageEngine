#pragma once

#include "GE_pch.h"

#include "../VulkanManager/GpuBuffer.h"
#include "../VulkanManager/VulkanCore.h"

#include "Runtime/core/IntervalJob.h"
#include "Runtime/core/Math/Math.h"

#include "Mesh.h"

namespace GE
{
    struct VertexInstanceData
    {
        alignas(16) float4x4 model;
        alignas(16) float4x4 t_inv_model;
    };

    // vertex data can be tightly packed
    struct Vertex
    {
        float3 position = {0.0f, 0.0f, 0.0f};
        float3 normal   = {0.0f, 0.0f, 0.0f};
        float3 tangent  = {0.0f, 0.0f, 0.0f};
        uint   flags    = 0;
        float2 uv0      = {0.0f, 0.0f};

        static inline VertexInputDescription& GetVertexInputDesc()
        {
            static VertexInputDescription description = {};
            static bool                   initialized = false;

            if (!initialized)
            {
                initialized = true;

                VkVertexInputBindingDescription vertexBinding = {};
                vertexBinding.binding                         = 0;
                vertexBinding.stride                          = sizeof(Vertex);
                vertexBinding.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;
                description.bindings.push_back(vertexBinding);

                // Position
                VkVertexInputAttributeDescription positionAttribute = {};
                positionAttribute.binding                           = 0;
                positionAttribute.location                          = 0;
                positionAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
                positionAttribute.offset                            = offsetof(Vertex, position);
                description.attributes.push_back(positionAttribute);

                // Normal
                VkVertexInputAttributeDescription normalAttribute = {};
                normalAttribute.binding                           = 0;
                normalAttribute.location                          = 1;
                normalAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
                normalAttribute.offset                            = offsetof(Vertex, normal);
                description.attributes.push_back(normalAttribute);

                // Tangent
                VkVertexInputAttributeDescription tangentAttribute = {};
                tangentAttribute.binding                           = 0;
                tangentAttribute.location                          = 2;
                tangentAttribute.format                            = VK_FORMAT_R32G32B32_SFLOAT;
                tangentAttribute.offset                            = offsetof(Vertex, tangent);
                description.attributes.push_back(tangentAttribute);

                // flags
                VkVertexInputAttributeDescription flagAttribute = {};
                flagAttribute.binding                           = 0;
                flagAttribute.location                          = 3;
                flagAttribute.format                            = VK_FORMAT_R32_UINT;
                flagAttribute.offset                            = offsetof(Vertex, flags);
                description.attributes.push_back(flagAttribute);

                // UV0
                VkVertexInputAttributeDescription texcoordAttribute = {};
                texcoordAttribute.binding                           = 0;
                texcoordAttribute.location                          = 4;
                texcoordAttribute.format                            = VK_FORMAT_R32G32_SFLOAT;
                texcoordAttribute.offset                            = offsetof(Vertex, uv0);
                description.attributes.push_back(texcoordAttribute);
            }

            return description;
        }
    };

    struct TriangleMeshData
    {
        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;

        ~TriangleMeshData()
        {
        }

        inline uint32_t GetTriangleCount() const { return m_indices.size() / 3; }
        inline uint32_t GetVertexCount() const { return m_vertices.size(); }
        inline uint32_t GetIndexCount() const { return m_indices.size(); }
    };

    class TriangleMeshResource;

    class TriangleMesh : public Mesh
    {
        GE_MESH_COMMON(TriangleMesh);
        const uint c_maxInstancePerDraw = 64;
        const uint c_vertexBindId       = 0;
        const uint c_instanceBindId     = 1;

    public:
        inline uint32_t GetTriangleCount() const;
        inline uint32_t GetVertexCount() const;
        inline uint32_t GetIndexCount() const;

        inline VkBuffer GetVertexBuffer() { return m_vertexBuffer.GetBuffer(); }
        inline VkBuffer GetIndexBuffer() { return m_indexBuffer.GetBuffer(); }

        void Inspect() override;
        void Deserialize(const json& data) override;
        json Serialize() const override;

        Bounds3f& BBox() override;

        void SetupRenderPipeline(GraphicsRenderPipeline& pipeline) override;
        void SetupPassResources(GraphicsPassUnit& unit) override;
        void RunRenderPass(MeshRenderPassData data) override;

        void Activate();
        void Deactivate();

        inline void SetMeshResource(std::shared_ptr<TriangleMeshResource> mesh) { m_meshResource = mesh; }

    private:
        static VkDescriptorSetLayoutBinding GetInstanceDataDescriptorSetLayoutBinding()
        {
            return {VkInit::GetDescriptorSetLayoutBinding(
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, 0)};
        }

        bool Update();

    private:
        const Time::Miliseconds c_updateInterval = Time::Miliseconds(2000);
        const Time::Miliseconds c_deactiveThres  = Time::Miliseconds(5000);
        Time::TimeStamp         m_tLastUsed;
        IntervalJob             m_updateJob;

        bool      m_uploaded = false;
        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;

        std::shared_ptr<TriangleMeshResource> m_meshResource;

        ImGuiFileDialog m_fileDialogInstance;
    };
} // namespace GE