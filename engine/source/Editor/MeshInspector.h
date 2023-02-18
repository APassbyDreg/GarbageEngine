#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    class ToggleMeshInspectorEvent : public Event
    {
        EVENT_IMPLEMENTATION_COMMON(None, EventCategoryApplication, ToggleMeshInspector);
        std::string ToString() { return "Menu: Toggle Mesh Inspector"; }
    };

    class MeshInspectorLayer : public Layer
    {
    public:
        MeshInspectorLayer() : Layer("Mesh Inspector") {}
        ~MeshInspectorLayer() {}

        inline void OnEvent(Event& e) override
        {
            if (e.GetName() == "ToggleMeshInspector")
            {
                m_open      = !m_open;
                e.m_handled = true;
            }
        }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            // Mesh List
            if (m_open)
            {
                ImGui::Begin("Mesh", &m_open);

                Application& app     = Application::GetInstance();
                fs::path     workdir = app.GetWorkDirectory();

                auto&& mesh_list = MeshManager::GetMeshList();

                if (ImGui::Button("Add Mesh.."))
                    ImGui::OpenPopup("ge_add_mesh_popup");
                if (ImGui::BeginPopup("ge_add_mesh_popup"))
                {
                    m_MeshNameToCreate = "";

                    for (auto&& name : MeshManager::GetSupportedMesh())
                    {
                        if (ImGui::Selectable(name.c_str()))
                        {
                            m_MeshNameToCreate = name;
                            m_fileDialogInstance.OpenDialog(
                                "ChooseFileDlgKey", "Choose File", ".ge.mesh,.*", workdir.string().c_str());
                        }
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::CollapsingHeader("Mesh List"))
                {
                    uint idx = 0;
                    for (auto&& mat : mesh_list)
                    {
                        bool selected = m_selectedMeshID == idx;
                        if (ImGui::Selectable(mat->GetAlias().c_str(), selected))
                        {
                            m_selectedMeshID = idx * (selected ? -1 : 1);
                        }
                        idx++;
                    }
                }

                if (m_selectedMeshID >= 0 && ImGui::CollapsingHeader("Mesh Content"))
                {
                    mesh_list[m_selectedMeshID]->Inspect();
                }

                HandleCreateMesh();

                ImGui::End();
            }
        }

        inline void HandleCreateMesh()
        {
            if (!m_MeshNameToCreate.empty())
            {
                std::string mat_type = m_MeshNameToCreate.substr(0, 1);
                std::string mat_name = m_MeshNameToCreate.substr(1, m_MeshNameToCreate.size() - 1);

                if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
                {
                    if (m_fileDialogInstance.IsOk())
                    {
                        std::string filepath = m_fileDialogInstance.GetFilePathName();

                        MeshManager::CreateMesh(m_MeshNameToCreate, filepath);

                        GE_CORE_INFO("Created Mesh {} at {}", m_MeshNameToCreate, filepath);
                    }
                    m_fileDialogInstance.Close();
                    m_MeshNameToCreate = "";
                }
            }
        }

    private:
        ImGuiFileDialog m_fileDialogInstance;
        bool            m_open             = false;
        int             m_selectedMeshID   = -1;
        std::string     m_MeshNameToCreate = "";
    };
} // namespace GE