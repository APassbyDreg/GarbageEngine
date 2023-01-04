#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Json.h"
#include "Runtime/core/Math/Math.h"

#include "Runtime/function/Log/LogSystem.h"

#include "ComponentBase.h"

namespace GE
{
    class Entity;

    class GE_API ComponentFactory : public Singleton<ComponentFactory>
    {
    public:
        inline void AttachComponent(std::string name, Entity& e)
        {
            EnsureInit();
            GE_CORE_ASSERT(m_componentEmptyFactories.find(name) != m_componentEmptyFactories.end(),
                           "[ComponentFactory::AttachComponent] component {} not registered",
                           name);
            m_componentEmptyFactories[name](e);
        }

        inline void AttachComponent(std::string name, const json& data, Entity& e)
        {
            EnsureInit();
            GE_CORE_ASSERT(m_componentJsonFactories.find(name) != m_componentJsonFactories.end(),
                           "[ComponentFactory::AttachComponent] component {} not registered",
                           name);
            m_componentJsonFactories[name](data, e);
        }

        inline void DetachComponent(std::string name, Entity& e)
        {
            EnsureInit();
            GE_CORE_CHECK(m_componentRemovers.find(name) != m_componentRemovers.end(),
                          "[ComponentFactory::DetachComponent] component {} not registered",
                          name);
            m_componentRemovers[name](e);
        }

        inline const std::vector<std::string>& GetSupportedComponents()
        {
            EnsureInit();
            return m_supportedComponents;
        }

    private:
        void EnsureInit();

        std::map<std::string, std::function<void(Entity&)>>              m_componentEmptyFactories = {};
        std::map<std::string, std::function<void(const json&, Entity&)>> m_componentJsonFactories  = {};
        std::map<std::string, std::function<void(Entity&)>>              m_componentRemovers       = {};
        std::vector<std::string>                                         m_supportedComponents     = {};
        bool                                                             m_initialized             = false;
    };
} // namespace GE