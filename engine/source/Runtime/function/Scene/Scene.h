#pragma once

#include "GE_pch.h"

#include "Entity.h"

#include "Runtime/function/Scene/Components/ComponentFactory.h"

namespace GE
{
    class GE_API Scene
    {
    public:
        Scene() {};
        ~Scene() {};

        inline entt::registry& GetRegistry() { return m_registry; }

        std::shared_ptr<Entity> CreateEntity(std::string tagname = "unnamed entity", uint layer = 0, uint tag = 0);

        void InspectStructure();
        void InspectFocusedEntity();

        json Serialize() const;
        void Deserialize(const json& data);

    private:
        int                                  m_focusEntityID = -1;
        entt::registry                       m_registry;
        std::vector<std::shared_ptr<Entity>> m_entities;
    };
} // namespace GE