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
        int idx = 0;
        for (auto& e : m_entities)
        {
            TagComponent& tag = e->GetComponent<TagComponent>();
            if (ImGui::Selectable(tag.m_name.c_str(), m_focusEntityID == idx))
            {
                m_focusEntityID = idx;
                GE_CORE_INFO("scene structure select {}", idx);
            }
            idx++;
        }
    }

    void Scene::InspectFocusedEntity()
    {
        if (m_focusEntityID >= 0 && m_focusEntityID < m_entities.size())
        {
            m_entities[m_focusEntityID]->InspectComponents();
        }
        else
        {
            m_focusEntityID = -1;
        }
    }
} // namespace GE