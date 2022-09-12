#pragma once

#include "GE_pch.h"

/* ------------------------ register components here ------------------------ */
#include "Camera.h"
#include "InstancedMesh.h"
#include "Tag.h"
#include "Transform.h"

#include "Runtime/core/Base/Singleton.h"

namespace GE
{
    class Entity;

    class ComponentFactory : public Singleton<ComponentFactory>
    {
    public:
        inline void AttachComponent(std::string name, const json& data, Entity& e)
        {
            EnsureInit();
            GE_CORE_ASSERT(m_componentFactories.find(name) != m_componentFactories.end(),
                           "[ComponentFactory::AttachComponent] component {} not registered",
                           name);
            m_componentFactories[name](data, e);
        }

        inline void DetachComponent(std::string name, Entity& e)
        {
            EnsureInit();
            GE_CORE_CHECK(m_componentRemovers.find(name) != m_componentRemovers.end(),
                          "[ComponentFactory::DetachComponent] component {} not registered",
                          name);
            m_componentRemovers[name](e);
        }

        inline std::map<std::string, std::function<void(const json&, Entity&)>>& GetFactoriesMap()
        {
            EnsureInit();
            return m_componentFactories;
        }
        inline std::map<std::string, std::function<void(Entity&)>>& GetRemoversMap()
        {
            EnsureInit();
            return m_componentRemovers;
        }

    private:
        void EnsureInit();

        std::map<std::string, std::function<void(const json&, Entity&)>> m_componentFactories = {};
        std::map<std::string, std::function<void(Entity&)>>              m_componentRemovers  = {};
        bool                                                             m_initialized        = false;
    };
} // namespace GE