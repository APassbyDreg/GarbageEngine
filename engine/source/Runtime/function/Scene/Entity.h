#pragma once

#include "GE_pch.h"

#include "Runtime/core/ECS.h"

#include "Runtime/function/Log/LogSystem.h"
#include "Runtime/function/Scene/Components/ComponentFactory.h"

#include "Systems/SystemBase.h"

namespace GE
{
    class Entity
    {
    public:
        Entity(entt::registry& reg) : m_srcReg(reg) { m_entityID = reg.create(); }
        Entity(const Entity& entity) : m_srcReg(entity.m_srcReg), m_entityID(entity.m_entityID) {}
        Entity(const json& data, entt::registry& reg) : m_srcReg(reg)
        {
            m_entityID = reg.create();
            Deserialize(data);
        }

        json Serialize() const;
        void Deserialize(const json& data);

        /* --------------------------- components --------------------------- */

        using ComponentIteratorFunc      = std::function<void(ComponentBase&)>;
        using ComponentConstIteratorFunc = std::function<void(const ComponentBase&)>;

        template<std::derived_from<ComponentBase> T, typename... TArgs>
        void AddComponent(TArgs&&... args)
        {
            GE_CORE_ASSERT(HasComponent<T>() == false, "[Entity::AddComponent] Component already exists");
            m_srcReg.emplace<T>(m_entityID, std::forward<TArgs>(args)...);
            m_compIters[T::GetNameStatic()] = [&, this](ComponentIteratorFunc f) {
                T& comp = GetComponent<T>();
                f(comp);
            };
            m_compConstIters[T::GetNameStatic()] = [&, this](ComponentConstIteratorFunc f) {
                const T& comp = GetComponent<T>();
                f(comp);
            };
        }

        template<std::derived_from<ComponentBase> T>
        void RemoveComponent()
        {
            GE_CORE_ASSERT(HasComponent<T>() == true, "[Entity::RemoveComponent] Component not found");
            m_srcReg.remove<T>(m_entityID);
            m_compIters.erase(T::GetNameStatic());
            m_compConstIters.erase(T::GetNameStatic());
        }

        template<std::derived_from<ComponentBase> T>
        bool HasComponent()
        {
            return m_srcReg.any_of<T>(m_entityID);
        }

        template<std::derived_from<ComponentBase> T>
        T& GetComponent()
        {
            GE_CORE_ASSERT(HasComponent<T>() == true, "[Entity::GetComponent] Component not found");
            return m_srcReg.get<T>(m_entityID);
        }

        void IterateComponent(ComponentIteratorFunc f);
        void IterateComponentConst(ComponentConstIteratorFunc f) const;

        /* ----------------------------- systems ---------------------------- */
        template<std::derived_from<SystemBase> T>
        void AddSystem()
        {
            std::shared_ptr<SystemBase> system = std::make_shared<T>(m_srcReg, m_entityID);
            m_systems.push_back(system);
        }

        void OnUpdate(const double dt);

    private:
        template<std::derived_from<ComponentBase> T>
        ComponentBase& GetComponentAsBase()
        {
            return GetComponent<T>();
        }

        entt::entity                                                           m_entityID;
        entt::registry&                                                        m_srcReg;
        std::map<std::string, std::function<void(ComponentIteratorFunc)>>      m_compIters;
        std::map<std::string, std::function<void(ComponentConstIteratorFunc)>> m_compConstIters;
        std::vector<std::shared_ptr<SystemBase>>                               m_systems;
    };
} // namespace GE