#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/Json.h"

namespace GE
{
#define GE_SETTINGS_COMMON(name) \
public: \
    name() \
    {} \
    name(const json& data) \
    { \
        Deserialize(data); \
    } \
    inline std::string GetName() const override \
    { \
        return #name; \
    } \
    inline static std::string GetNameStatic() \
    { \
        return #name; \
    }

#define LABEL_WITH_NAME(text) (std::string(text) + "##Settings_" + GetName()).c_str()

    class SettingsBase
    {
    public:
        virtual void        Inspect()                     = 0;
        virtual json        Serialize() const             = 0;
        virtual void        Deserialize(const json& data) = 0;
        virtual std::string GetName() const               = 0;
    };
} // namespace GE