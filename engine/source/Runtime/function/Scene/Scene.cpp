#include "Scene.h"

namespace GE
{
    std::shared_ptr<Entity> Scene::CreateEntity(std::string tagname, uint layer, uint tag)
    {
        auto e = std::make_shared<Entity>(m_registry);
        e->AddComponent<TagComponent>(tagname, layer, tag);
        m_entities.push_back(e);
        return e;
    }

    void Scene::InspectStructure()
    {
        strcpy(m_nameBuffer, m_name.c_str());
        ImGui::InputText("Scene Name", m_nameBuffer, 256);
        m_name = m_nameBuffer;

        ImGui::Separator();
        ImGui::Text("Entities");

        int idx = 0;
        for (auto& e : m_entities)
        {
            TagComponent& tag  = e->GetComponent<TagComponent>();
            const char*   name = tag.m_name.c_str();
            if (tag.m_name.empty())
            {
                name = "unnamed entity";
            }
            if (ImGui::Selectable(name, m_focusEntityID == idx))
            {
                m_focusEntityID = idx;
            }
            idx++;
        }
    }

    void Scene::InspectFocusedEntity()
    {
        if (m_focusEntityID >= 0 && m_focusEntityID < m_entities.size())
        {
            std::vector<std::string> remove_list = {};

            uint idx = 0;
            m_entities[m_focusEntityID]->IterateComponent([&](ComponentBase& comp, Entity& e) {
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
                ComponentFactory::GetInstance().DetachComponent(name, *m_entities[m_focusEntityID]);
            }
        }
        else
        {
            m_focusEntityID = -1;
        }
    }

    json Scene::Serialize() const
    {
        json entities = json::array();
        for (auto&& e : m_entities)
        {
            entities.push_back(e->Serialize());
        }

        json root;
        root["name"]     = m_name;
        root["entities"] = entities;

        return root;
    }

    void Scene::Deserialize(const json& data)
    {
        m_name = data["name"].get<std::string>();
        for (auto& edata : data["entities"])
        {
            m_entities.push_back(std::make_shared<Entity>(edata, m_registry));
        }
    }

    void Scene::Load(fs::path path)
    {
        if (m_resource != nullptr) // save previous work
        {
            m_resource->SaveData(Serialize());
        }
        m_resource = ResourceManager::GetInstance().GetResource<JsonResource>(path, JsonIdentifier::SCENE_DESCRIPTION);
        Deserialize(m_resource->GetData());
    }

    void Scene::Save(fs::path path, const bool save_as)
    {
        json data  = Serialize();
        bool saved = false;

        if (m_resource != nullptr) // save previous work
        {
            m_resource->SaveData(data);
            saved = true;
        }

        if (path != "" && (save_as || m_resource == nullptr)) // save a new file
        {
            m_resource =
                ResourceManager::GetInstance().GetResource<JsonResource>(path, JsonIdentifier::SCENE_DESCRIPTION);
            m_resource->SaveData(data);
            saved = true;
        }

        if (!saved)
        {
            GE_CORE_WARN("[Scene::Save] Failed to save because both previous and given path is empty!");
        }
    }
} // namespace GE