#include "TriangleMesh.h"

#include "../ShaderManager/HLSLCompiler.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/TriangleMeshResource.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "Runtime/Application.h"

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
        // alias
        char buf[256];
        strcpy(buf, m_alias.c_str());
        ImGui::InputText("Alias", buf, 256);
        m_alias = buf;

        // mesh file selection
        if (ImGui::Button("Select Triangle Mesh"))
        {
            Application& app     = Application::GetInstance();
            fs::path     workdir = app.GetWorkDirectory();
            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseFileDlgKey", "Choose File", ".ge.trianglemesh,.*", workdir.string().c_str());
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();
                if (filepath.ends_with(".ge.trianglemesh"))
                {
                    m_meshResource = ResourceManager::GetResource<TriangleMeshResource>(filepath);
                }
                else if (filepath.ends_with(".obj"))
                {
                    std::string target_filepath = filepath.substr(0, filepath.size() - 4) + ".ge.trianglemesh";
                    m_meshResource              = ResourceManager::GetResource<TriangleMeshResource>(target_filepath);
                    m_meshResource->FromObj(filepath);
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // mesh file name
        std::string mesh_path = "Unset";
        if (m_meshResource != nullptr)
        {
            mesh_path = m_meshResource->GetFilePath().string();
        }
        ImGui::SameLine();
        ImGui::Text("Source File: %s", mesh_path.c_str());

        // mesh info
        if (m_meshResource != nullptr)
        {
            auto&& data = m_meshResource->GetData();
            ImGui::Text("%d vertices, %d indices", data.GetVertexCount(), data.GetIndexCount());
        }
    }

    json TriangleMesh::Serialize()
    {
        json data;
        data["alias"] = m_alias;
        data["mesh"]  = m_meshResource == nullptr ? "" : m_meshResource->GetFilePath().string();
        return data;
    }

    void TriangleMesh::Deserialize(const json& data)
    {
        GE_CORE_CHECK(data["type"].get<std::string>() == GetName(),
                      "[TriangleMesh::Deserialize] Error input type '{}'",
                      data["type"].get<std::string>());
        if (data["type"].get<std::string>() == GetName())
        {
            m_alias          = data["alias"].get<std::string>();
            auto&& mesh_path = data["mesh"].get<std::string>();
            if (!mesh_path.empty())
            {
                m_meshResource = ResourceManager::GetResource<TriangleMeshResource>(mesh_path);
            }
        }
    }
} // namespace GE