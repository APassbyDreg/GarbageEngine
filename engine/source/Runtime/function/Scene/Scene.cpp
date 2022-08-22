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
            m_entities[m_focusEntityID]->IterateComponent([](ComponentBase& comp) {
                comp.Inspect();
                ImGui::Separator();
            });
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
        root["name"]        = m_name;
        root["entities"]    = entities;
        root["GE_ASSET_ID"] = "scene";

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
} // namespace GE