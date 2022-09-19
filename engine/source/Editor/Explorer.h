#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    class ExplorerLayer : public Layer
    {
    public:
        ExplorerLayer() : Layer("Explorer") {}
        ~ExplorerLayer() {}

        inline void UpdateWorkSpace()
        {
            fs::path workspace = Application::GetInstance().GetWorkDirectory();
            if (workspace != m_root)
            {
                m_root    = workspace;
                m_current = workspace;
            }
        }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            UpdateWorkSpace();

            ImGui::SetCurrentContext(ctx);
            ImGui::Begin("Content Explorer");
            if (m_root == "")
            {
                ImGui::Text("No workspace selected, please ");
                ImGui::SameLine();
                if (ImGui::Button("Open Workspace"))
                {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Workspace", nullptr, ".");
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey"))
                {
                    if (ImGuiFileDialog::Instance()->IsOk())
                    {
                        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                        GE_CORE_INFO("Opening workspace: {}", filePath);
                        Application::GetInstance().SetWorkDirectory(filePath);
                    }
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else
            {
                if (m_current == "")
                    m_current = m_root;
                // goback
                if (m_current != m_root)
                {
                    if (ImGui::Button(".."))
                    {
                        m_current = m_current.parent_path();
                    }
                }
                // list files
                std::vector<fs::path> contents = std::vector<fs::path>();
                for (auto&& entry : fs::directory_iterator(m_current))
                {
                    contents.push_back(entry.path());
                }
                std::sort(contents.begin(), contents.end(), [](const fs::path& a, const fs::path& b) {
                    if (fs::is_directory(a) && !fs::is_directory(b))
                    {
                        return true;
                    }
                    else if (!fs::is_directory(a) && fs::is_directory(b))
                    {
                        return false;
                    }
                    else
                    {
                        return a.filename() < b.filename();
                    }
                });
                for (auto&& entry : contents)
                {
                    if (fs::is_directory(entry))
                    {
                        if (ImGui::Button(entry.filename().string().c_str()))
                        {
                            m_current = entry;
                        }
                    }
                    else
                    {
                        ImGui::Text(entry.filename().string().c_str());
                    }
                }
            }
            ImGui::End();
        }

    private:
        fs::path m_root;
        fs::path m_current;
    };
} // namespace GE