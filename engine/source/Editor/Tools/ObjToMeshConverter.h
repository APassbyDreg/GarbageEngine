#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    class ObjToMeshConverterToolLayer : public Layer
    {
        enum class SelectionTarget
        {
            OBJ,
            MESH
        };

    public:
        ObjToMeshConverterToolLayer() : Layer("ObjToMeshConverterTool") {}

        inline void SetOnExit(std::function<void()> onexit) { m_onexit = onexit; }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);
            if (m_isEnabled)
            {
                fs::path workdir = Application::GetInstance().GetWorkDirectory();

                ImGui::Begin("OBJ to Mesh Converter", &m_isEnabled);

                /* --------------------- path selection --------------------- */
                if (ImGui::Button("Select OBJ Path"))
                {
                    m_target = SelectionTarget::OBJ;
                    m_fileDialogInstance.OpenDialog(
                        "ChooseFileDlgKey", "Choose OBJ File", ".obj,.*", workdir.string().c_str());
                }
                HandleFileSelect();
                ImGui::SameLine();
                ImGui::Text(m_objPath.length() > 0 ? m_objPath.c_str() : "None");
                if (ImGui::Button("Select Mesh Path"))
                {
                    m_target = SelectionTarget::MESH;
                    m_fileDialogInstance.OpenDialog(
                        "ChooseFileDlgKey", "Choose Mesh File", ".ge.trianglemesh,.*", workdir.string().c_str());
                }
                HandleFileSelect();
                ImGui::SameLine();
                ImGui::Text(m_meshPath.length() > 0 ? m_meshPath.c_str() : "None");

                /* ----------------------- convert btn ---------------------- */
                if (ImGui::Button("Convert"))
                {
                    fs::path objPath  = m_objPath;
                    fs::path meshPath = m_meshPath;
                    if (fs::exists(objPath))
                    {
                        m_showTip = false;
                        Convert();
                    }
                    else
                    {
                        m_showTip = true;
                    }
                }

                if (m_showTip)
                {
                    ImGui::Text("Please select valid obj and mesh path");
                }

                ImGui::End();
            }
            else
            {
                m_onexit();
            }
        }

        inline void HandleFileSelect()
        {
            if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
            {
                if (m_fileDialogInstance.IsOk())
                {
                    std::string filepath = m_fileDialogInstance.GetFilePathName();
                    if (m_target == SelectionTarget::OBJ)
                    {
                        m_objPath = filepath;
                    }
                    else if (m_target == SelectionTarget::MESH)
                    {
                        m_meshPath = filepath;
                    }
                }
                m_fileDialogInstance.Close();
            }
        }

        inline void Convert()
        {
            auto mesh = ResourceManager::GetResource<TriangleMeshResource>(m_meshPath);
            mesh->FromObj(m_objPath);
            mesh->Save();
        }

    private:
        std::function<void()> m_onexit;

        ImGuiFileDialog m_fileDialogInstance;
        SelectionTarget m_target;
        bool            m_showTip  = false;
        std::string     m_objPath  = "";
        std::string     m_meshPath = "";
    };
} // namespace GE