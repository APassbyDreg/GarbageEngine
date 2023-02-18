#include "Material.h"

#include "Runtime/Application.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "Runtime/function/Render/Material/MaterialManager.h"

namespace GE
{
    json MaterialComponent::Serialize() const
    {
        auto&& val = m_core.GetValue();
        json   data;
        data["material"] = val->GetPath().string();
        data["type"]     = val->GetType();
        data["mode"]     = val->GetMode();
        return data;
    }

    void MaterialComponent::Deserialize(const json& data)
    {
        std::string path     = data["material"].get<std::string>();
        std::string type     = data["type"].get<std::string>();
        std::string mode     = data["mode"].get<std::string>();
        auto&&      material = MaterialManager::LoadMaterial(path);
        if (material == nullptr)
        {
            material = MaterialManager::CreateMaterial(type, mode, path);
        }
        m_core = material;
    }

    void MaterialComponent::Inspect()
    {
        auto&& material = m_core.GetValue();
        if (material != nullptr)
        {
            ImGui::Text("Selected instance: %s", material->GetPath().string().c_str());
            if (ImGui::Button(LABEL_WITH_NAME("Select Another")))
            {
                ImGui::OpenPopup("ge_material_select_existing");
            }
            ImGui::Separator();
            material->Inspect();
        }
        else
        {
            if (ImGui::Button(LABEL_WITH_NAME("Select Existing")))
            {
                ImGui::OpenPopup("ge_material_select_existing");
            }
            ImGui::SameLine();
            if (ImGui::Button(LABEL_WITH_NAME("Select From File")))
            {
                fs::path workdir = Application::GetInstance().GetWorkDirectory();
                GetImGuiFileDialogInstance().OpenDialog(
                    "ChooseFileDlgKey", "Choose Material Asset", ".ge.mat,.*", workdir.string().c_str());
            }
            HandleOpenFile();
        }

        // material popup
        if (ImGui::BeginPopup("ge_material_select_existing"))
        {
            if (ImGui::CollapsingHeader(LABEL_WITH_NAME("Forward Materials")))
            {
                for (auto&& mat : MaterialManager::GetForwardMaterialList())
                {
                    if (ImGui::Selectable(mat->GetAlias().c_str()))
                    {
                        m_core = mat;
                    }
                }
            }
            if (ImGui::CollapsingHeader(LABEL_WITH_NAME("Deferred Materials")))
            {
                for (auto&& mat : MaterialManager::GetDeferredMaterialList())
                {
                    if (ImGui::Selectable(mat->GetAlias().c_str()))
                    {
                        m_core = mat;
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

    void MaterialComponent::HandleOpenFile()
    {
        std::string filepath = "";
        if (GetImGuiFileDialogInstance().Display("ChooseFileDlgKey"))
        {
            if (GetImGuiFileDialogInstance().IsOk())
            {
                filepath = GetImGuiFileDialogInstance().GetFilePathName();
            }
            GetImGuiFileDialogInstance().Close();
        }
        if (filepath.ends_with(".ge.material"))
        {
            m_core = MaterialManager::LoadMaterial(filepath);
        }
    }

} // namespace GE