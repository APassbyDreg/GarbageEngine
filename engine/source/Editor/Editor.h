#pragma once

#include "GE.h"

#include "imgui.h"

namespace GE
{
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
