#include "Light.h"
#include <string.h>

namespace GE
{
    json LightComponent::Serialize() const
    {
        auto&& val = m_core.GetValue();
        json   data;
        data["type"]      = __type2name(val.type);
        data["color"]     = {val.color.x, val.color.y, val.color.z};
        data["intensity"] = val.intensity;
        if (val.type == 0) // point
        {
            data["range"]  = val.effect_range;
            data["radius"] = val.radius;
        }
        if (val.type == 1) // spot
        {
            data["range"]  = val.effect_range;
            data["aspect"] = val.aspect;
            data["spread"] = val.spread;
        }
        if (val.type == 2) // direction
        {
            data["num_cascade"] = val.num_cascade;
        }
        return data;
    }

    void LightComponent::Deserialize(const json& data)
    {
        LightComponentCore val;
        val.type      = __name2type(data["type"].get<std::string>());
        val.color     = {data["color"][0].get<float>(), data["color"][1].get<float>(), data["color"][2].get<float>()};
        val.intensity = data["intensity"].get<float>();

        if (val.type == 0) // point
        {
            val.effect_range = data["range"].get<float>();
            val.radius       = data["radius"].get<float>();
        }
        if (val.type == 1) // spot
        {
            val.effect_range = data["range"].get<float>();
            val.aspect       = data["aspect"].get<float>();
            val.spread       = data["spread"].get<float>();
        }
        if (val.type == 2) // direction
        {
            val.num_cascade = data["num_cascade"].get<float>();
        }
        m_core = val;
    }

    void LightComponent::Inspect()
    {
        auto [type, color, intensity, range, radius, aspect, spread, num_cascades] = m_core.GetValue();
        auto [old_type,
              old_color,
              old_intensity,
              old_range,
              old_radius,
              old_aspect,
              old_spread,
              old_num_cascades]                                                           = m_core.GetValue();
        bool changed                                                                      = false;

        const char* type_names[3] = {"point", "spot", "direction"};
        const int   type_count    = 3;
        int         type_id       = type;
        ImGui::Combo(LABEL_WITH_NAME("Type"), &type_id, type_names, type_count);
        type    = (LightType)type_id;
        changed = changed || type != old_type;

        ImGui::DragFloat3(LABEL_WITH_NAME("Color"), (float*)&color, 0.01, 0.0, 1.0);
        changed = changed || color != old_color;

        float speed = type == LIGHT_TYPE_DIRECTION ? 0.01 : 1.0;
        ImGui::DragFloat(LABEL_WITH_NAME("Intensity"), (float*)&intensity, speed);
        changed = changed || intensity != old_intensity;

        if (type == 0) // point
        {
            ImGui::DragFloat(LABEL_WITH_NAME("Effect Range"), &range, 1.0, 0.0, 10000.0);
            changed = changed || range != old_range;
            ImGui::DragFloat(LABEL_WITH_NAME("Radius"), &radius, 0.1, 0.0, 10.0);
            changed = changed || radius != old_radius;
        }
        if (type == 1) // spot
        {
            ImGui::DragFloat(LABEL_WITH_NAME("Effect Range"), &range);
            ImGui::DragFloat(LABEL_WITH_NAME("Spread"), &spread, 1.0, 1.0, 179.0);
            // convert to more comfortable control
            aspect = aspect < 1 ? 1 - (1 / aspect) : aspect - 1;
            ImGui::DragFloat(LABEL_WITH_NAME("Aspect"), &aspect, 0.1, -10.0, 10.0);
            aspect = aspect < 0 ? 1 / (aspect + 1) : aspect + 1;

            changed = changed || range != old_range;
            changed = changed || spread != old_spread;
            changed = changed || abs(aspect - old_aspect) < 1e-4;
        }
        if (type == 2) // direction
        {
            ImGui::DragInt(LABEL_WITH_NAME("Cascades"), &num_cascades, 1, 1, 4);
            changed = changed || num_cascades != old_num_cascades;
        }

        if (changed)
        {
            m_core = {type, color, intensity, range, radius, aspect, spread, num_cascades};
        }
    }
} // namespace GE