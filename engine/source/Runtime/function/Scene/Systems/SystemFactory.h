#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "SystemBase.h"

namespace GE
{
    class Entity;

    class SystemFactory : public Singleton<SystemFactory>
    {
    public:
        inline void AttachSystem(std::string name, Entity& e) { m_systemMap[name](e); }

    private:
        template<typename T>
        void RegisterSystem(std::string name)
        {
            m_systemMap[name] = [](Entity& e) { e.AddSystem<T>(); };
        }

#define REGISTER_TYPE(typename) RegisterComponent<typename>(#typename);
        inline void EnsureInit()
        {
            if (!m_initialized)
            {
                m_initialized = true;
            }
        }
#undef REGISTER_TYPE

        std::map<std::string, std::function<void(Entity&)>> m_systemMap;
        bool                                                m_initialized = false;
    };
} // namespace GE