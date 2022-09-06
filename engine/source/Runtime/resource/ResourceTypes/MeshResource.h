#pragma once

#include "GE_pch.h"

#include "../Resource.h"

#include "Runtime/function/Render/DataStructures/Mesh.h"

#include "ByteResource.h"

namespace GE
{
    /* layout:
     * 0  - 7  : version
     * 8  - 15 : vertex count
     * 16 - 24 : index count
     * [vertex plain data]
     * [index plain data]
     */
    class MeshResource : public Resource<Mesh>
    {
    public:
        MeshResource(fs::path file, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::MESH, file, use_cache, delayed_load),
            m_trueResource(file, ByteResourceMagicNumber::MESH, use_cache, delayed_load)
        {}

        void Load() override;
        void Save() override;

        void FromObj(fs::path file);

        uint64 m_version = 0;

    private:
        const int64 VERSION_OFFSET    = 0;
        const int64 VERTEX_CNT_OFFSET = VERSION_OFFSET + sizeof(uint64);
        const int64 IDXEX_CNT_OFFSET  = VERTEX_CNT_OFFSET + sizeof(uint64);
        const int64 VERTEX_OFFSET     = IDXEX_CNT_OFFSET + sizeof(uint64);

        ByteResource m_trueResource;
    };
} // namespace GE