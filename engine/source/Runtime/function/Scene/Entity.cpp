#include "Entity.h"
#include "Components/ComponentFactory.h"
#include "Systems/SystemFactory.h"

#include "Scene.h"

namespace GE
{
    Entity::Entity(Scene& sc, int eid) : m_scene(sc), m_entityID(eid), m_srcReg(sc.m_registry)
    {
        m_registryID = m_srcReg.create();
    }

    json Entity::Serialize() const
    {
        json components;
        IterateComponentConst(
            [&](const ComponentBase& comp, Entity& e) { components[comp.GetName()] = comp.Serialize(); });

        json systems = json::array();
        {
            for (auto& sys : m_systems)
            {
                systems.push_back(sys->GetName());
            }
        }

        json root;
        root["components"] = components;
        root["systems"]    = systems;
        root["id"]         = m_entityID;
        root["parent"]     = m_parentID;
        return root;
    }

    void Entity::Deserialize(const json& data)
    {
        for (auto& [name, component] : data["components"].items())
        {
            ComponentFactory::GetInstance().AttachComponent(name, component, *this);
        }
        for (auto& sys : data["systems"])
        {
            SystemFactory::GetInstance().AttachSystem(sys, *this);
        }
        m_entityID = data["id"].get<int>();
        m_parentID = data["parent"].get<int>();
        MarkChanged();
    }

    void Entity::OnUpdate(double dt)
    {
        for (auto& system : m_systems)
        {
            system->OnUpdate(dt);
        }
        MarkChanged();
    }

    void Entity::IterateComponent(ComponentIteratorFunc f)
    {
        for (auto& [name, iter] : m_compIters)
        {
            iter(f);
        }
        MarkChanged();
    }

    void Entity::IterateComponentConst(ComponentConstIteratorFunc f) const
    {
        for (auto& [name, iter] : m_compConstIters)
        {
            iter(f);
        }
    }

    std::shared_ptr<Entity> Entity::GetParent()
    {
        m_parent = (m_parent == nullptr) ? m_scene.GetEntityByID(m_parentID) : m_parent;
        return m_parent;
    }

    void Entity::SetParent(std::shared_ptr<Entity> e)
    {
        // wipe old parent info
        if (GetParent() != nullptr)
        {
            auto iter = m_parent->m_children.begin();
            while (iter != m_parent->m_children.end())
            {
                if (*iter == shared_from_this())
                {
                    m_parent->m_children.erase(iter);
                    break;
                }
                iter++;
            }
        }

        // write new parent info
        m_parent   = e;
        m_parentID = e->GetEntityID();
        e->m_children.push_back(shared_from_this());
    }

    void Entity::MarkChanged()
    {
        m_version = (m_version + 1) % (2 << 30);
        for (auto&& child : m_children)
        {
            child->MarkChanged();
        }
    }
} // namespace GE