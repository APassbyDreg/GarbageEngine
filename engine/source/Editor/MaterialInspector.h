#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include <string>

namespace GE
{
    class ToggleMaterialInspectorEvent : public Event
    {
        EVENT_IMPLEMENTATION_COMMON(None, EventCategoryApplication, ToggleMaterialInspector);
        std::string ToString() { return "Menu: Toggle Material Inspector"; }
    };

    class MaterialInspectorLayer : public Layer
    {
    public:
        MaterialInspectorLayer() : Layer("Material Inspector") {}
        ~MaterialInspectorLayer() {}

        inline void OnEvent(Event& e) override
        {
            if (e.GetName() == "ToggleMaterialInspector")
            {
                m_open      = !m_open;
                e.m_handled = true;
            }
        }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            // Material List
            if (m_open)
            {
                ImGui::Begin("Material", &m_open);

                Application& app     = Application::GetInstance();
                fs::path     workdir = app.GetWorkDirectory();

                auto&& forward_list  = MaterialManager::GetForwardMaterialList();
                auto&& deferred_list = MaterialManager::GetDeferredMaterialList();

                if (ImGui::Button("Add Material.."))
                    ImGui::OpenPopup("ge_add_component_popup");
                if (ImGui::BeginPopup("ge_add_component_popup"))
                {
                    m_materialNameToCreate = "";

                    if (ImGui::CollapsingHeader("Forward Materials"))
                    {
                        for (auto&& name : MaterialManager::GetSupportedMaterial<ForwardMaterial>())
                        {
                            if (ImGui::Selectable(name.c_str()))
                            {
                                m_materialNameToCreate = "F" + name;
                                m_fileDialogInstance.OpenDialog(
                                    "ChooseFileDlgKey", "Choose File", ".ge.mat,.*", workdir.string().c_str());
                            }
                        }
                    }

                    if (ImGui::CollapsingHeader("Deferred Materials"))
                    {
                        for (auto&& name : MaterialManager::GetSupportedMaterial<DeferredMaterial>())
                        {
                            if (ImGui::Selectable(name.c_str()))
                            {
                                m_materialNameToCreate = "D" + name;
                                m_fileDialogInstance.OpenDialog(
                                    "ChooseFileDlgKey", "Choose File", ".ge.mat,.*", workdir.string().c_str());
                            }
                        }
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::CollapsingHeader("Material List"))
                {
                    uint idx = 0;
                    for (auto&& mat : forward_list)
                    {
                        bool selected = m_selectedMaterialID == idx;
                        if (ImGui::Selectable(mat->GetAlias().c_str(), selected))
                        {
                            m_selectedMaterialID = idx * (selected ? -1 : 1);
                        }
                        idx++;
                    }
                    for (auto&& mat : deferred_list)
                    {
                        bool selected = m_selectedMaterialID == idx;
                        if (ImGui::Selectable(mat->GetAlias().c_str(), m_selectedMaterialID == idx))
                        {
                            m_selectedMaterialID = idx * (selected ? -1 : 1);
                        }
                        idx++;
                    }
                }

                if (m_selectedMaterialID >= 0 && ImGui::CollapsingHeader("Material Content"))
                {
                    if (m_selectedMaterialID < forward_list.size())
                    {
                        forward_list[m_selectedMaterialID]->Inspect();
                    }
                    else
                    {
                        deferred_list[m_selectedMaterialID - forward_list.size()]->Inspect();
                    }
                }

                HandleCreateMaterial();

                ImGui::End();
            }
        }

        inline void HandleCreateMaterial()
        {
            if (!m_materialNameToCreate.empty())
            {
                std::string mat_type = m_materialNameToCreate.substr(0, 1);
                std::string mat_name = m_materialNameToCreate.substr(1, m_materialNameToCreate.size() - 1);

                if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
                {
                    if (m_fileDialogInstance.IsOk())
                    {
                        std::string filepath = m_fileDialogInstance.GetFilePathName();

                        if (mat_type == "F")
                        {
                            MaterialManager::CreateMaterial(mat_name, "forward", filepath);
                        }
                        if (mat_type == "D")
                        {
                            MaterialManager::CreateMaterial(mat_name, "deferred", filepath);
                        }

                        GE_CORE_INFO("Created Material {} at {}", m_materialNameToCreate, filepath);
                    }
                    m_fileDialogInstance.Close();
                    m_materialNameToCreate = "";
                }
            }
        }

    private:
        ImGuiFileDialog m_fileDialogInstance;
        bool            m_open                 = false;
        int             m_selectedMaterialID   = -1;
        std::string     m_materialNameToCreate = "";
    };
} // namespace GE