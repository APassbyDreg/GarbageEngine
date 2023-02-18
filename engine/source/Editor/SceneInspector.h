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

        inline void InspectSceneStructure(std::shared_ptr<Scene> sc)
        {
            char name_buffer[256];
            strcpy(name_buffer, sc->GetName().c_str());
            ImGui::InputText("Scene Name", name_buffer, 256);
            sc->SetName(name_buffer);

            ImGui::Separator();
            ImGui::Text("Entities");

            int idx = 0;
            for (auto&& [eid, e] : sc->GetAllEntities())
            {
                TagComponent& tag  = e->GetComponent<TagComponent>();
                std::string   name = tag.GetTagName();
                if (name.empty())
                {
                    name = "unnamed entity";
                }
                if (ImGui::Selectable(name.c_str(), m_focusedEntityID == idx))
                {
                    m_focusedEntityID = eid;
                }
                idx++;
            }
        }

        inline void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            Application&           app = Application::GetInstance();
            std::shared_ptr<Scene> sc  = app.GetActiveScene();

            ImGui::Begin("Scene Inspector");
            InspectSceneStructure(sc);
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
        int  m_focusedEntityID   = -1;
    };
} // namespace GE