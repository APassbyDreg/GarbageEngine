#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "Editor/Tools/ObjToMeshConverter.h"

#include "MaterialInspector.h"
#include "MeshInspector.h"
#include "SceneInspector.h"


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
            Test,
            None
        };

        MenuLayer() : Layer("Menu") {}
        ~MenuLayer() {}
        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            Application& app = Application::GetInstance();

            ImGui::BeginMainMenuBar();
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Workspace"))
                {
                    m_pressedItem = MenuItem::OpenWorkspace;
                    m_fileDialogInstance.OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, ".");
                }
                if (ImGui::MenuItem("Open Scene"))
                {
                    m_pressedItem    = MenuItem::OpenScene;
                    fs::path workdir = app.GetWorkDirectory();
                    m_fileDialogInstance.OpenDialog(
                        "ChooseFileDlgKey", "Choose File", ".json,.*", workdir.string().c_str());
                }
                if (ImGui::MenuItem("Save"))
                {
                    m_pressedItem = MenuItem::Save;
                }
                if (ImGui::MenuItem("Save As"))
                {
                    m_pressedItem    = MenuItem::SaveAs;
                    fs::path workdir = app.GetWorkDirectory();
                    m_fileDialogInstance.OpenDialog(
                        "ChooseDirDlgKey", "Choose Directory to Save To", nullptr, workdir.string().c_str());
                }
                if (ImGui::MenuItem("Test"))
                {
                    m_pressedItem    = MenuItem::Test;
                    fs::path workdir = app.GetWorkDirectory();
                    m_fileDialogInstance.OpenDialog(
                        "ChooseFileDlgKey", "Choose File", ".json,.*", workdir.string().c_str());
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("OBJ to Mesh Converter"))
                {
                    std::shared_ptr<ObjToMeshConverterToolLayer> layer =
                        std::make_shared<ObjToMeshConverterToolLayer>();

                    layer->SetOnExit([&]() { app.AddPendingAction([&]() { app.PopLayer(layer); }); });
                    app.AddPendingAction([=]() { Application::GetInstance().PushLayer(layer); });
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Scene Settings"))
                {
                    auto e = ToggleSceneSettingsEvent();
                    Application::GetInstance().OnEvent(e);
                }

                if (ImGui::MenuItem("Material"))
                {
                    auto e = ToggleMaterialInspectorEvent();
                    Application::GetInstance().OnEvent(e);
                }

                if (ImGui::MenuItem("Mesh"))
                {
                    auto e = ToggleMeshInspectorEvent();
                    Application::GetInstance().OnEvent(e);
                }
                ImGui::EndMenu();
            }

            HandleOpenScene();
            HandleOpenWorkspace();
            HandleSave();
            HandleSaveAs();
            if (m_pressedItem == MenuItem::Test)
            {
                if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
                {
                    if (m_fileDialogInstance.IsOk())
                    {
                        std::string filepath = m_fileDialogInstance.GetFilePathName();
                        GE_APP_TRACE("Opening {}", filepath);
                        m_pressedItem = MenuItem::None;
                    }
                    m_fileDialogInstance.Close();
                }
            }

            ImGui::EndMainMenuBar();
        }

    private:
        ImGuiFileDialog m_fileDialogInstance;
        MenuItem m_pressedItem;

        inline void HandleOpenWorkspace()
        {
            if (m_pressedItem != MenuItem::OpenWorkspace)
                return;

            if (m_fileDialogInstance.Display("ChooseDirDlgKey"))
            {
                if (m_fileDialogInstance.IsOk())
                {
                    std::string workspace = m_fileDialogInstance.GetCurrentPath();
                    Application::GetInstance().SetWorkDirectory(workspace);
                    GE_CORE_INFO("Opening workspace: {}", workspace);
                    m_pressedItem = MenuItem::None;
                }
                m_fileDialogInstance.Close();
            }
        }

        inline void HandleOpenScene()
        {
            if (m_pressedItem != MenuItem::OpenScene)
                return;

            if (m_fileDialogInstance.Display("ChooseFileDlgKey"))
            {
                if (m_fileDialogInstance.IsOk())
                {
                    std::string filepath = m_fileDialogInstance.GetFilePathName();

                    auto scene = Application::GetInstance().GetActiveScene();
                    scene->Load(filepath);
                    GE_CORE_INFO("Open Scene: {0}", filepath);
                    m_pressedItem = MenuItem::None;
                }
                m_fileDialogInstance.Close();
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

            MeshManager::SaveAll();
            MaterialManager::SaveAll();
            m_pressedItem = MenuItem::None;
        }

        inline void HandleSaveAs()
        {
            if (m_pressedItem != MenuItem::SaveAs)
                return;
            if (m_fileDialogInstance.Display("ChooseDirDlgKey"))
            {
                if (m_fileDialogInstance.IsOk())
                {
                    std::string root  = m_fileDialogInstance.GetCurrentPath();
                    auto        scene = Application::GetInstance().GetActiveScene();
                    fs::path    path  = root / fs::path(scene->GetName() + ".json");
                    scene->Save(path, true);
                    Application::GetInstance().SetWorkDirectory(root);
                    GE_CORE_INFO("Saving to: {}", path.string());
                    m_pressedItem = MenuItem::None;
                }
                m_fileDialogInstance.Close();
            }
        }
    };
} // namespace GE