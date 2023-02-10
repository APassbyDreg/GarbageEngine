#include "Mesh.h"

#include "Runtime/Application.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "Runtime/function/Render/Mesh/MeshManager.h"

#include "Runtime/resource/ResourceTypes/TriangleMeshResource.h"

namespace GE
{
    json MeshComponent::Serialize() const
    {
        auto&& val = m_core.GetValue();
        json   data;
        data["type"] = val->GetType();
        data["mesh"] = val->GetPath().string();
        return data;
    }

    void MeshComponent::Deserialize(const json& data)
    {
        std::string path = data["mesh"].get<std::string>();
        std::string type = data["type"].get<std::string>();
        auto&&      mesh = MeshManager::LoadMesh(path);
        if (mesh == nullptr)
        {
            mesh = MeshManager::CreateMesh(type, path);
        }
        m_core = mesh;
    }

    void MeshComponent::Inspect()
    {
        auto&& mesh = m_core.GetValue();
        if (mesh != nullptr)
        {
            ImGui::Text("Selected instance: %s", mesh->GetPath().string().c_str());
            if (ImGui::Button(LABEL_WITH_NAME("Select Another")))
            {
                ImGui::OpenPopup("ge_mesh_select_existing");
            }
            ImGui::Separator();
            mesh->Inspect();
        }
        else
        {
            if (ImGui::Button(LABEL_WITH_NAME("Select Existing")))
            {
                ImGui::OpenPopup("ge_mesh_select_existing");
            }
            ImGui::SameLine();
            if (ImGui::Button(LABEL_WITH_NAME("Select From File")))
            {
                fs::path workdir = Application::GetInstance().GetWorkDirectory();
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey",
                                                        "Choose Mesh Asset",
                                                        ".ge.mesh,.obj,.ge.trianglemesh,.*",
                                                        workdir.string().c_str());
            }
            HandleOpenFile();
        }

        // mesh popup
        if (ImGui::BeginPopup("ge_mesh_select_existing"))
        {
            for (auto&& mesh : MeshManager::GetMeshList())
            {
                if (ImGui::Selectable(mesh->GetAlias().c_str()))
                {
                    m_core = mesh;
                }
            }
            ImGui::EndPopup();
        }
    }

    void MeshComponent::HandleOpenFile()
    {
        std::string filepath = "";
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filepath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (filepath.ends_with(".obj"))
        {
            std::string savepath = filepath.substr(0, filepath.size() - 4) + ".ge.trianglemesh";
            std::string meshpath = filepath.substr(0, filepath.size() - 4) + ".ge.mesh";
            auto&&      resource = ResourceManager::GetResource<TriangleMeshResource>(savepath);
            resource->FromObj(filepath);
            resource->Save();

            auto&& base_mesh = MeshManager::CreateMesh("TriangleMesh", meshpath);
            auto&& mesh      = std::reinterpret_pointer_cast<TriangleMesh>(base_mesh);
            mesh->SetMeshResource(resource);
            m_core = mesh;

            GE_CORE_INFO("Obj {} saved to {}, created mesh {}", filepath, savepath, meshpath);
        }
        else if (filepath.ends_with(".ge.trianglemesh"))
        {
            std::string meshpath  = filepath.substr(0, filepath.size() - 16) + ".ge.mesh";
            auto&&      resource  = ResourceManager::GetResource<TriangleMeshResource>(filepath);
            auto&&      base_mesh = MeshManager::CreateMesh("TriangleMesh", meshpath);
            auto&&      mesh      = std::reinterpret_pointer_cast<TriangleMesh>(base_mesh);
            mesh->SetMeshResource(resource);
            m_core = mesh;

            GE_CORE_INFO("Opened {}, created mesh {}", filepath, meshpath);
        }
        else if (filepath.ends_with(".ge.mesh"))
        {
            m_core = MeshManager::LoadMesh(filepath);
        }
    }

} // namespace GE