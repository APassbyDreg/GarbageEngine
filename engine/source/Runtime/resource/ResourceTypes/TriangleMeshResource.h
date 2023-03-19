#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

#include "Runtime/core/Math/Bounds.h"

#include "Runtime/function/Render/Mesh/TriangleMesh.h"
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
    class GE_API TriangleMeshResource : public Resource<TriangleMeshData>
    {
    public:
        TriangleMeshResource(fs::path file, bool init = false, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::MESH, file, init, use_cache, delayed_load),
            m_trueResource(file, ByteResourceMagicNumber::MESH, use_cache, delayed_load)
        {
            if (!delayed_load)
                Load();
            if (init && !m_valid)
                Initialize();
        }

        void Load() override;
        void Save() override;

        void FromObj(fs::path file);

        inline uint64&   Version() { return m_version; }
        inline Bounds3f& BBox() { return m_bbox; }
        inline size_t    GetVertexCount() { return m_data.m_vertices.size(); }
        inline size_t    GetIndexCount() { return m_data.m_indices.size(); }

        inline void Invalid() override
        {
            m_data  = {};
            m_valid = false;
        }
        inline void SaveData(const TriangleMeshData& data) override
        {
            m_data = data;
            Save();
        }

    private:
        const int64 c_versionOffset     = 0;
        const int64 c_vertexCountOffset = c_versionOffset + sizeof(uint64);
        const int64 c_indexCountOffset  = c_vertexCountOffset + sizeof(uint64);
        const int64 c_bboxOffset        = c_indexCountOffset + sizeof(uint64);
        const int64 c_vertexOffset      = c_bboxOffset + sizeof(Bounds3f);

        uint64       m_version = 0;
        Bounds3f     m_bbox;
        ByteResource m_trueResource;

    private:
        void CalculateBBox();
    };
} // namespace GE