#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "Runtime/function/Settings/SettingsBase.h"

namespace GE
{
    class TestSceneSetting : public SettingsBase
    {
    public:
        GE_SETTINGS_COMMON(TestSceneSetting);
        int    render_routine = 0;
        float3 clear_color    = {0, 0, 0};

        inline void Inspect() override
        {
            ImGui::DragInt(LABEL_WITH_NAME("Render Routine"), &render_routine, 1, 0, 1);
            ImGui::DragFloat3(LABEL_WITH_NAME("Clear Color"), (float*)&clear_color, 0.01, 0.0, 1.0);
        }

        inline json Serialize() const override
        {
            json data;
            data["Render Routine"] = render_routine;
            data["Clear Color"]    = {clear_color.x, clear_color.y, clear_color.z};
            return data;
        }

        inline void Deserialize(const json& data) override { render_routine = data["Render Routine"].get<int>(); }
    };
} // namespace GE