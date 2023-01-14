#include "InstancedMesh.h"

#include "Runtime/Application.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    inline void InstancedMeshComponent::HandleOpenFile()
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
            std::string savepath = filepath.substr(0, filepath.size() - 4) + ".ge.mesh";
            auto        resource = ResourceManager::GetResource<TriangleMeshResource>(savepath);
            resource->FromObj(filepath);
            resource->Save();
            m_core = resource;
            GE_CORE_INFO("Mesh {} saved to {}", filepath, savepath);
        }
        else if (filepath.ends_with(".ge.mesh"))
        {
            m_core = ResourceManager::GetResource<TriangleMeshResource>(filepath);
            GE_CORE_INFO("Opened mesh {}", filepath);
        }
    }

    void InstancedMeshComponent::Inspect()
    {
        std::shared_ptr<TriangleMeshResource> m_source = m_core;
        std::string                   path = m_source != nullptr ? m_source->GetFilePath().string() : "not selected";
        ImGui::Text("Source: %s", path.c_str());
        if (ImGui::Button("select"))
        {
            fs::path workdir = Application::GetInstance().GetWorkDirectory();
            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseFileDlgKey", "Choose Mesh Asset", ".ge.mesh,.*", workdir.string().c_str());
        }

        HandleOpenFile();
    }

} // namespace GE