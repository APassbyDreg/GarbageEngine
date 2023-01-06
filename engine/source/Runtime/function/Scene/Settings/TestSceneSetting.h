#pragma once

#include "GE_pch.h"

#include "Runtime/function/Settings/SettingsBase.h"

namespace GE
{
    class TestSceneSetting : public SettingsBase
    {
    public:
        GE_SETTINGS_COMMON(TestSceneSetting);
        int value = 0;

        inline void Inspect() override { ImGui::DragInt("Test Value", &value); }

        inline json Serialize() const override { return {"value", value}; }

        inline void Deserialize(const json& data) override { value = data["value"].get<int>(); }
    };
} // namespace GE