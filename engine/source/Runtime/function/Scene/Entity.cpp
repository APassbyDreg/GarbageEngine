#include "Entity.h"
#include "Components/ComponentFactory.h"
#include "Systems/SystemFactory.h"

namespace GE
{
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
} // namespace GE