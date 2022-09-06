#include "MeshResource.h"

#include "OBJ_Loader.h"

namespace GE
{
    void MeshResource::Load()
    {
        m_trueResource.Load();
        if (m_trueResource.m_magicnumber != ByteResourceMagicNumber::MESH)
        {
            GE_CORE_ERROR("[MeshResource::Load] Resource {} is not a mesh binary resource", m_filePath.string());
        }
        if (!m_trueResource.IsValid())
        {
            GE_CORE_ERROR("[MeshResource::Load] Error loading underlying resource");
            return;
        }

        // load data
        byte*  data = m_trueResource.GetData().data();
        uint64 vertex_cnt, index_cnt;
        // meta
        m_version  = *((uint64*)(data + VERSION_OFFSET));
        vertex_cnt = *((uint64*)(data + VERTEX_CNT_OFFSET));
        index_cnt  = *((uint64*)(data + IDXEX_CNT_OFFSET));
        // convert to mesh
        if (m_version == 0)
        {
            uint64 vertex_size = vertex_cnt * sizeof(Vertex);
            uint64 index_size  = index_cnt * sizeof(uint32_t);

            byte* vertex_start = data + VERTEX_OFFSET;
            m_data.vertices    = std::vector<Vertex>((Vertex*)vertex_start, (Vertex*)(vertex_start + vertex_size));
            byte* index_start  = vertex_start + vertex_size;
            m_data.indices     = std::vector<uint32_t>((uint32_t*)index_start, (uint32_t*)(index_start + index_size));
        }

        // clean up
        m_trueResource.Invalid(); // free data

        m_valid = true;
    }

    void MeshResource::Save()
    {
        GE_CORE_ASSERT(m_valid, "[MeshResource::Save] Trying to save invalid resource to {}", m_filePath.string());

        // compute
        uint64 vertex_cnt   = m_data.vertices.size();
        uint64 index_cnt    = m_data.indices.size();
        uint64 vertex_size  = vertex_cnt * sizeof(Vertex);
        uint64 index_size   = index_cnt * sizeof(uint32_t);
        uint64 full_size    = 3 * sizeof(uint64) + vertex_size + index_size;
        uint64 index_offset = VERTEX_OFFSET + vertex_size;

        std::vector<byte> bytedata = std::vector<byte>(full_size);
        byte*             data     = bytedata.data();
        // meta
        *((uint64*)(data + VERSION_OFFSET))    = m_version;
        *((uint64*)(data + VERTEX_CNT_OFFSET)) = vertex_cnt;
        *((uint64*)(data + IDXEX_CNT_OFFSET))  = index_cnt;
        // data
        memcpy(data + VERTEX_OFFSET, m_data.vertices.data(), vertex_size);
        memcpy(data + index_offset, m_data.indices.data(), index_size);

        // save and clean
        m_trueResource.SaveData(bytedata);
        m_trueResource.Invalid();
    }

    void MeshResource::FromObj(fs::path file)
    {
        objl::Loader loader;
        bool         success = loader.LoadFile(file.string());
        GE_CORE_CHECK(success, "[MeshResource::FromObj] Error loading obj file {}", file.string());

		// convert format
        for (objl::Mesh& m : loader.LoadedMeshes)
        {
            for (unsigned int& idx : m.Indices)
            {
                objl::Vertex v = m.Vertices[idx];
                Vertex       vert;
                vert.normal   = float3(v.Normal.X, v.Normal.Y, v.Normal.Z);
                vert.position = float3(v.Position.X, v.Position.Y, v.Position.Z);
                vert.uv0      = float2(v.TextureCoordinate.X, v.TextureCoordinate.Y);
                m_data.vertices.push_back(vert);
            }
            m_data.indices = m.Indices;
        }
		
		GE_CORE_INFO("[MeshResource::FromObj] loaded {} vertices and {} indices from {}", m_data.vertices.size(), m_data.indices.size(), file.string());
        m_valid = true;
    }
} // namespace GE