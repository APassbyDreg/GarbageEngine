#pragma once

#include "GE.h"
#include "imgui.h"

#include "Runtime/function/Scene/Components/Camera.h"
#include "Runtime/function/Scene/Components/Transform.h"

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
            static bool _show_demo_window = true;
            if (_show_demo_window)
                ImGui::ShowDemoWindow(&_show_demo_window);

            // custom window
            static bool popen = true;
            ImGui::Begin("Test", &popen);
            ImGui::Text("This is a test layer");
            if (ImGui::Button("Toggle demo window"))
            {
                _show_demo_window = !_show_demo_window;
            }
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
} // namespace GE