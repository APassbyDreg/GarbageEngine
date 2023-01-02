#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/Base/WatchedValue.h"
#include "Runtime/core/ECS.h"
#include "Runtime/core/Math/Math.h"
#include "Runtime/core/json.h"

namespace GE
{
    template<typename T>
    json SerializeComponent(const T& component)
    {
        return component.Serialize();
    }

    template<typename T>
    T DeserializeComponent(const json& data)
    {
        T component;
        component.Deserialize(data);
        return component;
    }

    template<typename T>
    void InspectComponent(T& component)
    {
        component.Inspect();
    }

    class Entity;
    using EntityCallback = std::function<void(Entity&)>;

#define GE_COMPONENT_COMMON(comp) \
public: \
    inline std::string GetName() const override \
    { \
        return #comp; \
    } \
    inline static std::string GetNameStatic() \
    { \
        return #comp; \
    } \
    inline void AddUpdatedCallback(EntityCallback cb) \
    { \
        m_core.AddCallback([=]() { cb(*(this->m_entity)); }); \
    } \
    comp(std::shared_ptr<Entity> e, const json& data) : ComponentBase(e) \
    { \
        Deserialize(data); \
    } \
    comp(std::shared_ptr<Entity> e) : ComponentBase(e) \
    {} \
    inline comp##Core GetCoreValues() \
    { \
        return m_core.GetValue(); \
    } \
    WatchedValue<comp##Core> m_core; \
    using CoreType = comp##Core;

    class ComponentBase
    {
    public:
        ComponentBase(std::shared_ptr<Entity> e) : m_entity(e) {}

        virtual void        Inspect()                     = 0;
        virtual void        Deserialize(const json& data) = 0;
        virtual json        Serialize() const             = 0;
        virtual std::string GetName() const               = 0;

        inline std::shared_ptr<Entity> GetEntity() { return m_entity; }

    protected:
        std::shared_ptr<Entity> m_entity;
    };
} // namespace GE