#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/Math/Math.h"
#include "Runtime/core/json.h"


#define GE_COMPONENT_COMMON(comp) \
    std::string        GetName() const override { return #comp; } \
    static std::string GetNameStatic() { return #comp; } \
    comp(const json& data) { Deserialize(data); }

namespace GE
{
    template<typename T>
    json SerializeComponentInstance(const T& component)
    {
        return component.SerializeInstance();
    }

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

    class ComponentBase
    {
    public:
        virtual void        Inspect()                     = 0;
        virtual void        Deserialize(const json& data) = 0;
        virtual json        Serialize() const             = 0;
        virtual std::string GetName() const               = 0;

        json SerializeInstance()
        {
            json root;
            root["name"]        = GetName();
            root["data"]        = Serialize();
            root["GE_ASSET_ID"] = "component instance";
            return root;
        }
    };
} // namespace GE