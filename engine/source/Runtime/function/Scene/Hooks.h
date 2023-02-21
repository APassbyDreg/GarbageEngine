#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Base/WithIdentifier.h"

#include "Components/ComponentBase.h"

namespace GE
{
    class Entity;
    using EntityCallback = std::function<void(Entity&)>;

    template<std::derived_from<ComponentBase> T>
    class ComponentHook : public Singleton<ComponentHook<T>>
    {
    public:
        static void AddConstructHook(EntityCallback hook, UniqueIdentifier scene_id)
        {
            ComponentHook<T>::GetInstance().m_constructHooks[scene_id].push_back(hook);
        }
        static void AddDestructHook(EntityCallback hook, UniqueIdentifier scene_id)
        {
            ComponentHook<T>::GetInstance().m_destructHooks[scene_id].push_back(hook);
        }
        static void AddChangedHook(EntityCallback hook, UniqueIdentifier scene_id)
        {
            ComponentHook<T>::GetInstance().m_changedHooks[scene_id].push_back(hook);
        }
        static void CallConstructHooks(Entity& e, UniqueIdentifier scene_id)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_constructHooks[scene_id])
                hook(e);
        }
        static void CallDestructHooks(Entity& e, UniqueIdentifier scene_id)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_destructHooks[scene_id])
                hook(e);
        }
        static void CallChangedHooks(Entity& e, UniqueIdentifier scene_id)
        {
            for (auto& hook : ComponentHook<T>::GetInstance().m_changedHooks[scene_id])
                hook(e);
        }

    private:
        std::map<UniqueIdentifier, std::vector<EntityCallback>> m_constructHooks, m_destructHooks, m_changedHooks;
    };
}; // namespace GE