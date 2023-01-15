#include "TriangleMesh.h"

#include "../ShaderManager/HLSLCompiler.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/TriangleMeshResource.h"

namespace GE
{
    void TriangleMesh::Activate()
    {
        GE_CORE_ASSERT(m_meshResource != nullptr, "Error trying to activate an invalid TriangleMesh");

        m_tLastUsed = Time::CurrentTime();

        if (!m_updateJob.IsRunning())
        {
            m_updateJob.Run(std::bind(&TriangleMesh::Update, this), c_updateInterval);
        }

        if (!m_uploaded)
        {
            auto& data = m_meshResource->GetData();

            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
            auto vertex_info =
                VkInit::GetBufferCreateInfo(sizeof(Vertex) * data.GetVertexCount(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            auto index_info =
                VkInit::GetBufferCreateInfo(sizeof(uint32_t) * data.GetIndexCount(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

            m_vertexBuffer.Alloc(vertex_info, alloc_info);
            m_indexBuffer.Alloc(index_info, alloc_info);

            m_vertexBuffer.UploadAs(data.m_vertices);
            m_indexBuffer.UploadAs(data.m_indices);
            m_uploaded = true;
        }
    }

    void TriangleMesh::Deactivate()
    {
        m_vertexBuffer.Destroy();
        m_indexBuffer.Destroy();
        m_uploaded = false;
    }

    bool TriangleMesh::Update()
    {
        if (m_uploaded && Time::CurrentTime() - m_tLastUsed > c_deactiveThres)
        {
            Deactivate();
        }
        return false;
    }

    Bounds3f& TriangleMesh::BBox() { return m_meshResource->BBox(); }

    void TriangleMesh::SetupPipeline(GraphicsRenderPipeline& pipeline)
    {
        {
            fs::path     path     = fs::path(Config::shader_dir) / "Passes/Mesh/TriangleMesh.gsf";
            HLSLCompiler compiler = {ShaderType::VERTEX};
            pipeline.m_shaders.push_back(compiler.Compile(path.string(), "VSMain"));
        }
        {
            VertexInputDescription desc = Vertex::GetVertexInputDesc();
            pipeline.m_vertexInputState =
                VkInit::GetPipelineVertexInputStateCreateInfo(desc.bindings, desc.attributes, desc.flags);
        }
    }

    void TriangleMesh::RunRenderPass(MeshRenderPassData data)
    {
        Activate();

        auto&& [cmd, layout, num_instance] = data;

        VkDeviceSize offsets[] = {0};
        VkBuffer     buffers[] = {m_vertexBuffer.GetBuffer()};
        vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(cmd, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, m_meshResource->GetVertexCount(), num_instance, 0, 0, 0);
    }

    void TriangleMesh::Inspect()
    {
        ImGui::Text("Source File: %s", m_meshResource->GetFilePath().string().c_str());
        ImGui::Text("Mesh File: %s", m_resource->GetFilePath().string().c_str());
    }

    json TriangleMesh::Serialize()
    {
        json data;
        data["mesh"] = m_meshResource->GetFilePath().string();
        return data;
    }

    void TriangleMesh::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetName(),
                      "[TriangleMesh::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetName())
        {
            m_meshResource = ResourceManager::GetResource<TriangleMeshResource>(data["mesh"].get<std::string>());
        }
    }
} // namespace GE