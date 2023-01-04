#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

#include "Runtime/core/Math/Bounds.h"
#include "Runtime/function/Render/DataStructures/Mesh.h"
#include "Runtime/function/Render/VulkanManager/GpuBuffer.h"

#include "ByteResource.h"

namespace GE
{
    /* layout:
     * 0  - 7  : version
     * 8  - 15 : vertex count
     * 16 - 23 : index count
     * 24 - 47 : bbox
     * [vertex plain data]
     * [index plain data]
     */
    class GE_API MeshResource : public Resource<Mesh>
    {
    public:
        MeshResource(fs::path file, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::MESH, file, use_cache, delayed_load),
            m_trueResource(file, ByteResourceMagicNumber::MESH, use_cache, delayed_load)
        {}

        void Load() override;
        void Save() override;

        void FromObj(fs::path file);

        inline uint64&   Version() { return m_version; }
        inline Bounds3f& BBox() { return m_bbox; }
        inline VkBuffer  GetVertexBuffer() { return m_vertexBuffer.GetBuffer(); }
        inline VkBuffer  GetIndexBuffer() { return m_indexBuffer.GetBuffer(); }
        inline size_t    GetVertexCount() { return m_data.vertices.size(); }
        inline size_t    GetIndexCount() { return m_data.indices.size(); }

    private:
        void ToGpu();

    private:
        const int64 c_versionOffset     = 0;
        const int64 c_vertexCountOffset = c_versionOffset + sizeof(uint64);
        const int64 c_indexCountOffset  = c_vertexCountOffset + sizeof(uint64);
        const int64 c_bboxOffset        = c_indexCountOffset + sizeof(uint64);
        const int64 c_vertexOffset      = c_bboxOffset + sizeof(Bounds3f);

        uint64       m_version = 0;
        Bounds3f     m_bbox;
        ByteResource m_trueResource;

        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;

    private:
        void CalculateBBox();
    };
} // namespace GE