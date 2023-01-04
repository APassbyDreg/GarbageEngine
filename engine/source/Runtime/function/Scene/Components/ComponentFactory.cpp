#include "ComponentFactory.h"

#include "../Entity.h"
#include "../Hooks.h"

/* ------------------------ register components here ------------------------ */
#include "Camera.h"
#include "InstancedMesh.h"
#include "Renderer.h"
#include "Tag.h"
#include "Transform.h"

namespace GE
{
#define REGISTER_TYPE(typename) \
    m_componentRemovers[#typename]       = [](Entity& e) { e.RemoveComponent<typename>(); }; \
    m_componentEmptyFactories[#typename] = [](Entity& e) { e.AddComponent<typename>(); }; \
    m_componentJsonFactories[#typename]  = [](const json& data, Entity& e) { e.AddComponent<typename>(data); }; \
    m_supportedComponents.push_back(#typename);

    void ComponentFactory::EnsureInit()
    {
        if (!m_initialized)
        {
            REGISTER_TYPE(CameraComponent);
            REGISTER_TYPE(TagComponent);
            REGISTER_TYPE(TransformComponent);
            REGISTER_TYPE(InstancedMeshComponent);
            REGISTER_TYPE(RendererComponent)
            m_initialized = true;
        }
    }
} // namespace GE