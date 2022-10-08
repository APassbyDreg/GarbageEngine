#pragma once

#include "GE_pch.h"

#include "Runtime/core/ECS.h"

#include "Runtime/function/Log/LogSystem.h"

#include "Components/ComponentFactory.h"
#include "Systems/SystemBase.h"

namespace GE
{
    class Scene;

    class Entity : public std::enable_shared_from_this<Entity>
    {
        friend class Scene;

    public:
        Entity(Scene& sc, int eid);
        Entity(Scene& sc, const json& data);

        json Serialize() const;
        void Deserialize(const json& data);

        /* --------------------------- components --------------------------- */

        using ComponentIteratorFunc      = std::function<void(ComponentBase&, Entity&)>;
        using ComponentConstIteratorFunc = std::function<void(const ComponentBase&, Entity&)>;

        template<std::derived_from<ComponentBase> T, typename... TArgs>
        void AddComponent(TArgs&&... args)
        {
            if (HasComponent<T>())
            {
                GE_CORE_WARN("[Entity::AddComponent] Component '{}' already exists, doing nothing", T::GetNameStatic());
                return;
            }

            m_srcReg.emplace<T>(m_registryID, shared_from_this(), std::forward<TArgs>(args)...);
            m_compIters[T::GetNameStatic()] = [&, this](ComponentIteratorFunc f) {
                T& comp = GetComponent<T>();
                f(comp, *this);
            };
            m_compConstIters[T::GetNameStatic()] = [&, this](ComponentConstIteratorFunc f) {
                const T& comp = GetComponent<T>();
                f(comp, *this);
            };
            MarkChanged();
        }

        template<std::derived_from<ComponentBase> T>
        void RemoveComponent()
        {
            if (!HasComponent<T>())
            {
                GE_CORE_WARN("[Entity::RemoveComponent] Component '{}' not found, doing nothing", T::GetNameStatic());
                return;
            }
            m_srcReg.remove<T>(m_registryID);
            m_compIters.erase(T::GetNameStatic());
            m_compConstIters.erase(T::GetNameStatic());
            MarkChanged();
        }

        template<std::derived_from<ComponentBase> T>
        bool HasComponent() const
        {
            return m_srcReg.any_of<T>(m_registryID);
        }

        template<std::derived_from<ComponentBase> T>
        T& GetComponent()
        {
            GE_CORE_ASSERT(HasComponent<T>(), "[Entity::GetComponent] Component '{}' not found", T::GetNameStatic());
            MarkChanged(); // NOTE: conservatively increament version
            return m_srcReg.get<T>(m_registryID);
        }

        template<std::derived_from<ComponentBase> T>
        const T& GetComponent() const
        {
            GE_CORE_ASSERT(HasComponent<T>(), "[Entity::GetComponent] Component '{}' not found", T::GetNameStatic());
            return m_srcReg.get<T>(m_registryID);
        }

        void IterateComponent(ComponentIteratorFunc f);
        void IterateComponentConst(ComponentConstIteratorFunc f) const;

        /* ----------------------------- systems ---------------------------- */
        template<std::derived_from<SystemBase> T>
        void AddSystem()
        {
            std::shared_ptr<SystemBase> system = std::make_shared<T>(shared_from_this());
            m_systems.push_back(system);
            MarkChanged();
        }

        void OnUpdate(const double dt);

        /* ------------------------ entity basic info ----------------------- */
        inline uint GetVersion() const { return m_version; }
        inline int  GetEntityID() const { return m_entityID; }

        inline Scene&           GetScene() { return m_scene; }
        std::shared_ptr<Entity> GetParent();

    private:
        template<std::derived_from<ComponentBase> T>
        ComponentBase& GetComponentAsBase()
        {
            return GetComponent<T>();
        }

        inline void MarkChanged() { m_version = (m_version + 1) % (2 << 30); }

        Scene&          m_scene;
        uint            m_version  = 0;
        int             m_entityID = -1; // valid entity id has to be greater than zero
        int             m_parentID = -1; // negative value indecates no parent
        entt::registry& m_srcReg;
        std::map<std::string, std::function<void(ComponentIteratorFunc)>>      m_compIters;
        std::map<std::string, std::function<void(ComponentConstIteratorFunc)>> m_compConstIters;
        std::vector<std::shared_ptr<SystemBase>>                               m_systems;
        entt::entity                                                           m_registryID;
    };
} // namespace GE