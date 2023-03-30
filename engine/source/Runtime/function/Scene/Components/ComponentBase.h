#pragma once

#include "GE_pch.h"

#include "imgui.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "Runtime/core/Base/Serializable.h"
#include "Runtime/core/Base/WatchedValue.h"
#include "Runtime/core/ECS.h"
#include "Runtime/core/Log/LogSystem.h"
#include "Runtime/core/Math/Math.h"
#include "Runtime/core/json.h"

namespace GE
{
    class Entity;
    using UpdateCallback = std::function<void()>;

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
    inline void AddUpdatedCallback(UpdateCallback cb) \
    { \
        m_core.AddCallback([=]() { cb(); }); \
    } \
    comp(std::shared_ptr<Entity> e, const json& data) : ComponentBase(e) \
    { \
        Deserialize(data); \
    } \
    comp(std::shared_ptr<Entity> e) : ComponentBase(e) \
    {} \
    inline comp##Core GetCoreValue() const \
    { \
        return m_core.GetValue(); \
    } \
    WatchedValue<comp##Core> m_core; \
    using CoreType = comp##Core;

#define LABEL_WITH_NAME(text) (std::string(text) + "##Component_" + GetName()).c_str()

    class ComponentBase : public Serializable<json>
    {
    public:
        ComponentBase(std::shared_ptr<Entity> e) : m_entity(e) {}

        virtual void        Inspect()                     = 0;
        virtual std::string GetName() const               = 0;

        inline std::shared_ptr<Entity> GetEntity() { return m_entity; }

    protected:
        std::shared_ptr<Entity> m_entity;
    };

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
} // namespace GE