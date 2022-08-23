#pragma once

#include "GE_pch.h"

/* ------------------------ register components here ------------------------ */
#include "Camera.h"
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
            m_componentFactories[name](data, e);
        }

    private:
        template<typename T>
        void RegisterComponent(std::string name)
        {
            m_componentFactories[name] = [](const json& data, Entity& e) { e.AddComponent<T>(data); };
        }

#define REGISTER_TYPE(typename) RegisterComponent<typename>(#typename);
        inline void EnsureInit()
        {
            if (!m_initialized)
            {
                REGISTER_TYPE(CameraComponent);
                REGISTER_TYPE(TagComponent);
                REGISTER_TYPE(TransformComponent);
                m_initialized = true;
            }
        }
#undef REGISTER_TYPE

        std::map<std::string, std::function<void(const json&, Entity&)>> m_componentFactories;
        bool                                                             m_initialized = false;
    };
} // namespace GE