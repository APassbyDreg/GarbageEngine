#pragma once

#include "GE_pch.h"

#include "Runtime/core/ECS.h"

#include "Runtime/function/Components/Components.h"
#include "Runtime/function/Log/LogSystem.h"

namespace GE
{
    class Entity
    {
    public:
        Entity(entt::registry& reg) : m_srcReg(reg) { m_entityID = reg.create(); }
        Entity(const Entity& entity) : m_srcReg(entity.m_srcReg), m_entityID(entity.m_entityID) {}

        template<typename T>
        void AddComponent(T& component)
        {
            GE_CORE_ASSERT(HasComponent<T>() == false, "[Entity::AddComponent] Component already exists!");
            m_srcReg.emplace<T>(m_entityID, component);
            m_inspectionList[typeid(T)] = [&, this]() {
                T& component = GetComponent<T>();
                InspectComponent(component);
            };
        }

        template<typename T, typename... TArgs>
        void AddComponent(TArgs&&... args)
        {
            GE_CORE_ASSERT(HasComponent<T>() == false, "[Entity::AddComponent] Component already exists");
            m_srcReg.emplace<T>(m_entityID, std::forward<TArgs>(args)...);
            m_inspectionList[typeid(T)] = [&, this]() {
                T& component = GetComponent<T>();
                InspectComponent(component);
            };
        }

        template<typename T>
        void RemoveComponent()
        {
            GE_CORE_ASSERT(HasComponent<T>() == true, "[Entity::RemoveComponent] Component not found");
            m_srcReg.remove<T>(m_entityID);
            m_inspectionList.erase(typeid(T));
        }

        template<typename T>
        bool HasComponent()
        {
            return m_srcReg.any_of<T>(m_entityID);
        }

        template<typename T>
        T& GetComponent()
        {
            GE_CORE_ASSERT(HasComponent<T>() == true, "[Entity::GetComponent] Component not found");
            return m_srcReg.get<T>(m_entityID);
        }

        void InspectComponents()
        {
            for (auto& [type, inspect] : m_inspectionList)
            {
                inspect();
                ImGui::Separator();
            }
        }

    private:
        entt::entity                                     m_entityID;
        entt::registry&                                  m_srcReg;
        std::map<std::type_index, std::function<void()>> m_inspectionList = {};
    };
} // namespace GE