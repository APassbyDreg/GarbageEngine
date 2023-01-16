#include "TriangleMeshResource.h"

#include "OBJ_Loader.h"

#include "Runtime/function/Render/VulkanManager/VulkanCreateInfoBuilder.h"

namespace GE
{
    void TriangleMeshResource::Load()
    {
        m_trueResource.Load();

        if (!m_trueResource.IsValid())
        {
            return;
        }
        if (m_trueResource.m_magicnumber != ByteResourceMagicNumber::MESH)
        {
            GE_CORE_ERROR("[TriangleMeshResource::Load] Resource {} is not a mesh binary resource",
                          m_filePath.string());
        }
        if (!m_trueResource.IsValid())
        {
            GE_CORE_ERROR("[TriangleMeshResource::Load] Error loading underlying resource");
            return;
        }

        // load data
        byte*  data = m_trueResource.GetData().data();
        uint64 vertex_cnt, index_cnt;
        // meta
        m_version  = *((uint64*)(data + c_versionOffset));
        vertex_cnt = *((uint64*)(data + c_vertexCountOffset));
        index_cnt  = *((uint64*)(data + c_indexCountOffset));
        m_bbox     = *((Bounds3f*)(data + c_bboxOffset));
        // convert to mesh
        if (m_version == 0)
        {
            uint64 vertex_size = vertex_cnt * sizeof(Vertex);
            uint64 index_size  = index_cnt * sizeof(uint32_t);

            byte* vertex_start = data + c_vertexOffset;
            m_data.m_vertices    = std::vector<Vertex>((Vertex*)vertex_start, (Vertex*)(vertex_start + vertex_size));
            byte* index_start  = vertex_start + vertex_size;
            m_data.m_indices     = std::vector<uint32_t>((uint32_t*)index_start, (uint32_t*)(index_start + index_size));
        }

        // clean up
        m_trueResource.Invalid(); // free data

        m_valid = true;
    }

    void TriangleMeshResource::Save()
    {
        GE_CORE_ASSERT(
            m_valid, "[TriangleMeshResource::Save] Trying to save invalid resource to {}", m_filePath.string());

        // compute
        uint64 vertex_cnt   = m_data.m_vertices.size();
        uint64 index_cnt    = m_data.m_indices.size();
        uint64 vertex_size  = vertex_cnt * sizeof(Vertex);
        uint64 index_size   = index_cnt * sizeof(uint32_t);
        uint64 index_offset = c_vertexOffset + vertex_size;
        uint64 full_size    = c_vertexOffset + vertex_size + index_size;

        std::vector<byte> bytedata = std::vector<byte>(full_size);
        byte*             data     = bytedata.data();
        // meta
        *((uint64*)(data + c_versionOffset))     = m_version;
        *((uint64*)(data + c_vertexCountOffset)) = vertex_cnt;
        *((uint64*)(data + c_indexCountOffset))  = index_cnt;
        *((Bounds3f*)(data + c_bboxOffset))      = m_bbox;
        // data
        memcpy(data + c_vertexOffset, m_data.m_vertices.data(), vertex_size);
        memcpy(data + index_offset, m_data.m_indices.data(), index_size);

        // save and clean
        m_trueResource.SaveData(bytedata);
        m_trueResource.Invalid();
    }

    void TriangleMeshResource::FromObj(fs::path file)
    {
        objl::Loader loader;
        bool         success = loader.LoadFile(file.string());
        GE_CORE_CHECK(success, "[TriangleMeshResource::FromObj] Error loading obj file {}", file.string());

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
                m_data.m_vertices.push_back(vert);
            }
            m_data.m_indices = m.Indices;
        }

        GE_CORE_INFO("[TriangleMeshResource::FromObj] loaded {} vertices and {} indices from {}",
                     m_data.m_vertices.size(),
                     m_data.m_indices.size(),
                     file.string());

        // postprocess
        CalculateBBox();

        m_valid = true;
    }

    void TriangleMeshResource::CalculateBBox()
    {
        auto& vertices = m_data.m_vertices;
        GE_CORE_CHECK(vertices.size() > 0, "[TriangleMeshResource::CalculateBBox] calculating bbox from empty mesh");

        m_bbox = Bounds3f(vertices[0].position);
        for (int i = 1; i < vertices.size(); i++)
        {
            m_bbox += Bounds3f(vertices[i].position);
        }
    }
} // namespace GE