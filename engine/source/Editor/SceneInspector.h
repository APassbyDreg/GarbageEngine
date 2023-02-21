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
            for (auto&& [eid, e] : sc->m_entities)
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

        inline void InspectFocusedEntity(std::shared_ptr<Scene> sc)
        {
            std::shared_ptr<Entity> focused_entity = sc->GetEntityByID(m_focusedEntityID);

            if (focused_entity == nullptr)
            {
                m_focusedEntityID = -1;
                return;
            }

            std::vector<std::string> add_list    = {};
            std::vector<std::string> remove_list = {};

            // add new component
            if (ImGui::Button("Add Component.."))
                ImGui::OpenPopup("ge_add_component_popup");
            if (ImGui::BeginPopup("ge_add_component_popup"))
            {
                for (auto&& name : ComponentFactory::GetInstance().GetSupportedComponents())
                {
                    if (ImGui::Selectable(name.c_str()))
                        ComponentFactory::GetInstance().AttachComponent(name, *focused_entity);
                }
                ImGui::EndPopup();
            }

            // parent entity selector
            if (ImGui::Button("Select Parent.."))
                ImGui::OpenPopup("ge_parent_entity_popup");
            if (ImGui::BeginPopup("ge_parent_entity_popup"))
            {
                for (auto [eid, entity] : sc->m_entities)
                {
                    std::string display_name =
                        std::to_string(eid) + ": " + entity->GetComponent<TagComponent>().GetCoreValue().name;
                    if (ImGui::Selectable(display_name.c_str()))
                    {
                        if (focused_entity->GetEntityID() != eid)
                            focused_entity->SetParent(entity);
                    }
                }
                ImGui::EndPopup();
            }

            // display parent
            ImGui::SameLine();
            std::shared_ptr<Entity> parent = focused_entity->GetParent();
            std::string             parent_text =
                "Parent: " + ((parent == nullptr) ? "None" : parent->GetComponent<TagComponent>().GetCoreValue().name);
            ImGui::Text(parent_text.c_str());

            // per component
            uint idx = 0;
            focused_entity->IterateComponent([&](ComponentBase& comp, Entity& e) {
                if (ImGui::CollapsingHeader(comp.GetName().c_str()))
                {
                    if (comp.GetName() != TagComponent::GetNameStatic())
                    {
                        ImGui::PushID(idx);
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
                        if (ImGui::Button("remove", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                        {
                            remove_list.push_back(comp.GetName());
                        }
                        ImGui::PopStyleColor(3);
                        ImGui::PopID();
                    }
                    comp.Inspect();
                }
                ImGui::Separator();
                idx++;
            });

            for (auto&& name : remove_list)
            {
                ComponentFactory::GetInstance().DetachComponent(name, *focused_entity);
            }
        }

        inline void InspectSceneSettings(std::shared_ptr<Scene> sc)
        {
            for (auto&& [name, setting] : sc->m_sceneSettings)
            {
                if (ImGui::CollapsingHeader(name.c_str()))
                {
                    setting->Inspect();
                }
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
            InspectFocusedEntity(sc);
            ImGui::End();

            if (m_openSceneSettings)
            {
                ImGui::Begin("Scene Settings", &m_openSceneSettings);
                InspectSceneSettings(sc);
                ImGui::End();
            }
        }

        bool m_openSceneSettings = false;
        int  m_focusedEntityID   = -1;
    };
} // namespace GE