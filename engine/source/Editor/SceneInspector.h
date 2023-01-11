#pragma once

#include "GE.h"

#include "imgui.h"

namespace GE
{
    class ToggleSceneSettingsEvent : public Event
    {
        EVENT_IMPLEMENTATION_COMMON(None, EventCategoryApplication, ToggleSceneSettings);
        std::string ToString() { return "Menu: Toggle Scene Settings"; }
    };

    class SceneInspectorLayer : public Layer
    {
    public:
        SceneInspectorLayer() : Layer("Scene Inspector") {}
        ~SceneInspectorLayer() {}

        inline void OnEvent(Event& e) override
        {
            if (e.GetName() == "ToggleSceneSettings")
            {
                m_openSceneSettings = !m_openSceneSettings;
                e.m_handled         = true;
            }
        }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            Application&           app = Application::GetInstance();
            std::shared_ptr<Scene> sc  = app.GetActiveScene();

            ImGui::Begin("Scene Inspector");
            sc->InspectStructure();
            ImGui::End();

            ImGui::Begin("Entity Inspector");
            sc->InspectFocusedEntity();
            ImGui::End();

            if (m_openSceneSettings)
            {
                ImGui::Begin("Scene Settings", &m_openSceneSettings);
                sc->InspectSettings();
                ImGui::End();
            }
        }

        bool m_openSceneSettings = false;
    };
} // namespace GE