#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "Components/ComponentBase.h"

namespace GE
{
    class Entity;
    using EntityCallback = std::function<void(Entity&)>;

    template<std::derived_from<ComponentBase> T>
    class ComponentHook : public Singleton<ComponentHook<T>>
    {
    public:
        static void AddConstructHook(EntityCallback hook, std::string sc_name)
        {
            ComponentHook<T>::GetInstance().m_constructHooks[sc_name].push_back(hook);
        }
        static void AddDestructHook(EntityCallback hook, std::string sc_name)
        {
            ComponentHook<T>::GetInstance().m_destructHooks[sc_name].push_back(hook);
        }
        static void AddChangedHook(EntityCallback hook, std::string sc_name)
        {
            ComponentHook<T>::GetInstance().m_changedHooks[sc_name].push_back(hook);
        }
        static void CallConstructHooks(Entity& e, std::string sc_name)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_constructHooks[sc_name])
                hook(e);
        }
        static void CallDestructHooks(Entity& e, std::string sc_name)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_destructHooks[sc_name])
                hook(e);
        }
        static void CallChangedHooks(Entity& e, std::string sc_name)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_changedHooks[sc_name])
                hook(e);
        }

    private:
        std::map<std::string, std::vector<EntityCallback>> m_constructHooks, m_destructHooks, m_changedHooks;
    };
}; // namespace GE