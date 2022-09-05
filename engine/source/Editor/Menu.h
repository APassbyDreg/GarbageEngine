#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    class MenuLayer : public Layer
    {
    public:
        enum class MenuItem
        {
            OpenWorkspace,
            OpenScene,
            Save,
            SaveAs,
            None
        };

        MenuLayer() {}
        ~MenuLayer() {}
        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::BeginMainMenuBar();
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Open Workspace"))
                {
                    m_pressedItem = MenuItem::OpenWorkspace;
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, ".");
                }
                if (ImGui::MenuItem("Open Scene"))
                {
                    m_pressedItem    = MenuItem::OpenScene;
                    fs::path workdir = Application::GetInstance().GetWorkDirectory();
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseFileDlgKey", "Choose File", ".json,.*", workdir.string().c_str());
                }
                if (ImGui::MenuItem("Save"))
                {
                    m_pressedItem = MenuItem::Save;
                }
                if (ImGui::MenuItem("Save As"))
                {
                    m_pressedItem    = MenuItem::SaveAs;
                    fs::path workdir = Application::GetInstance().GetWorkDirectory();
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseDirDlgKey", "Choose Directory to Save To", nullptr, workdir.string().c_str());
                }
                ImGui::EndMenu();
            }

            HandleOpenScene();
            HandleOpenWorkspace();
            HandleSave();
            HandleSaveAs();

            ImGui::EndMainMenuBar();
        }

    private:
        MenuItem m_pressedItem;

        inline void HandleOpenWorkspace()
        {
            if (m_pressedItem != MenuItem::OpenWorkspace)
                return;

            if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string workspace = ImGuiFileDialog::Instance()->GetCurrentPath();
                    Application::GetInstance().SetWorkDirectory(workspace);
                    GE_CORE_INFO("Opening workspace: {}", workspace);
                }
                ImGuiFileDialog::Instance()->Close();
            }
        }

        inline void HandleOpenScene()
        {
            if (m_pressedItem != MenuItem::OpenScene)
                return;

            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();

                    auto scene = Application::GetInstance().GetActiveScene();
                    scene->Load(filepath);
                    GE_CORE_INFO("Open Scene: {0}", filepath);
                }
                ImGuiFileDialog::Instance()->Close();
            }
        }

        inline void HandleSave()
        {
            if (m_pressedItem != MenuItem::Save)
                return;
            fs::path workdir = Application::GetInstance().GetWorkDirectory();
            auto     scene   = Application::GetInstance().GetActiveScene();
            fs::path path    = workdir / fs::path(scene->GetName() + ".json");
            scene->Save(path, false);
        }

        inline void HandleSaveAs()
        {
            if (m_pressedItem != MenuItem::SaveAs)
                return;
            if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string root  = ImGuiFileDialog::Instance()->GetCurrentPath();
                    auto        scene = Application::GetInstance().GetActiveScene();
                    fs::path    path  = root / fs::path(scene->GetName() + ".json");
                    scene->Save(path, true);
                    Application::GetInstance().SetWorkDirectory(root);
                    GE_CORE_INFO("Saving to: {}", path.string());
                }
                ImGuiFileDialog::Instance()->Close();
            }
        }
    };
} // namespace GE