#include "InstancedMesh.h"

#include "Runtime/Application.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    inline void InstancedMeshComponent::HandleOpenFile()
    {
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string                   filepath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::shared_ptr<MeshResource> m_source =
                    ResourceManager::GetInstance().GetResource<MeshResource>(filepath);
                m_core = m_source;
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void InstancedMeshComponent::Inspect()
    {
        std::shared_ptr<MeshResource> m_source = m_core;
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