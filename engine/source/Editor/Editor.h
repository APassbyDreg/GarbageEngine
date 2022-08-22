#pragma once

#include "GE.h"

#include "imgui.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace GE
{
    class MenuLayer : public Layer
    {
    public:
        MenuLayer() {}
        ~MenuLayer() {}
        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::BeginMainMenuBar();
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Open Workspace"))
                {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, ".");
                }
                if (ImGui::MenuItem("Open Scene"))
                {
                    fs::path workdir = Application::GetInstance().GetWorkDirectory();
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseFileDlgKey", "Choose File", ".json,.*", workdir.string().c_str());
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    SaveScene();
                }
                ImGui::EndMenu();
            }

            HandleOpenScene();
            HandleOpenWorkspace();

            ImGui::EndMainMenuBar();
        }

    private:
        inline void HandleOpenScene()
        {
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    SaveScene();
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    Application::GetInstance().OpenScene(filePathName);
                    GE_CORE_INFO("Opening scene: {}", filePathName);
                }
                ImGuiFileDialog::Instance()->Close();
            }
        }

        inline void HandleOpenWorkspace()
        {
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

        inline void SaveScene()
        {
            fs::path workdir = Application::GetInstance().GetWorkDirectory();
            if (workdir != "")
            {
                auto          scene = Application::GetInstance().GetActiveScene();
                json          data  = scene->Serialize();
                std::ofstream file(workdir / fs::path(scene->GetName() + ".json"));
                file << data.dump();
                file.close();
            }
        }
    };

    class TestLayer : public Layer
    {
        int counter = 0;

    public:
        TestLayer() {}
        ~TestLayer() {}

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            // demo window
            bool _show_demo_window = true;
            ImGui::ShowDemoWindow(&_show_demo_window);

            // custom window
            ImGui::Begin("Test");
            ImGui::Text("This is a test layer");
            if (ImGui::Button("Add an basic entity"))
            {
                Application&            app = Application::GetInstance();
                std::shared_ptr<Scene>  sc  = app.GetActiveScene();
                std::shared_ptr<Entity> e   = sc->CreateEntity(std::format("Empty {}", counter), counter);
                counter++;
            }
            if (ImGui::Button("Add an camera entity"))
            {
                Application&            app = Application::GetInstance();
                std::shared_ptr<Scene>  sc  = app.GetActiveScene();
                std::shared_ptr<Entity> e   = sc->CreateEntity(std::format("Camera {}", counter), counter);
                e->AddComponent<TransformComponent>();
                e->AddComponent<CameraComponent>();
                counter++;
            }
            ImGui::End();
        }
    };

    class Editor : public Application
    {
    public:
        Editor();
        ~Editor();
    };

} // namespace GE
