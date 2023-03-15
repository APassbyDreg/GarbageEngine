#pragma once

#include "ComponentBase.h"

#include "Runtime/function/Render/Light/Light.h"

namespace GE
{
    struct LightComponentCore
    {
        LightType   type      = LIGHT_TYPE_POINT;
        float3      color     = {1.0, 1.0, 1.0};
        float       intensity = 1.0;
        // for spot & point
        float effect_range = 100.0;
        // for point
        float radius = 0.0;
        // for spot
        float aspect = 1.0;
        float spread = 30.0; // fov
        // for direction
        int num_cascade = 1;
    };

    class GE_API LightComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(LightComponent);

    public:
        json Serialize() const override;

        void Deserialize(const json& data) override;

        void Inspect() override;

    private:
        static LightType __name2type(std::string name)
        {
            if (name == "point")
                return LIGHT_TYPE_POINT;
            if (name == "spot")
                return LIGHT_TYPE_SPOT;
            if (name == "direction")
                return LIGHT_TYPE_DIRECTION;
            GE_CORE_WARN("Unrecognized light type name {}", name);
            return LIGHT_TYPE_UNKNOWN;
        }

        static std::string __type2name(LightType type)
        {
            switch (type)
            {
                case 0:
                    return "point";
                case 1:
                    return "spot";
                case 2:
                    return "direction";
                default:
                    break;
            };
            GE_CORE_WARN("Unrecognized light type id {}", (int)type);
            return "unknown";
        }
    };
} // namespace GE