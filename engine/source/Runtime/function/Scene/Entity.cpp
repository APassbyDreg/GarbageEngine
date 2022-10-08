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

    Entity::Entity(Scene& sc, const json& data) : m_scene(sc), m_srcReg(sc.m_registry)
    {
        m_registryID = m_srcReg.create();
        Deserialize(data);
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

    std::shared_ptr<Entity> Entity::GetParent() { return m_scene.GetEntityByID(m_parentID); }
} // namespace GE