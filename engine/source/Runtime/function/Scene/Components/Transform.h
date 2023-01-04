#pragma once

#include "ComponentBase.h"

namespace GE
{
    struct TransformComponentCore
    {
        float3 position = {0, 0, 0};
        float3 scale    = {1, 1, 1};
        quat   rotation = {1, 0, 0, 0};

        bool operator==(TransformComponentCore&& rhs)
        {
            return position == rhs.position && scale == rhs.scale && rotation == rhs.rotation;
        }

        operator std::tuple<float3, float3, quat>() { return {position, scale, rotation}; }
    };

    class TransformComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(TransformComponent);

    public:
        TransformComponent(std::shared_ptr<Entity> e, float3 pos, float3 scale, float3 rot) : ComponentBase(e)
        {
            m_core = {pos, scale, rot};
        }
        TransformComponent(std::shared_ptr<Entity> e, float3 pos, float3 scale, quat rot) : ComponentBase(e)
        {
            m_core = {pos, scale, rot};
        }

        inline json Serialize() const override
        {
            auto [position, scale, rotation] = m_core.GetValue();
            return {{"position", {position.x, position.y, position.z}},
                    {"scale", {scale.x, scale.y, scale.z}},
                    {"rotation", {rotation.x, rotation.y, rotation.z, rotation.w}}};
        }

        inline void Deserialize(const json& data) override
        {
            float3 position, scale;
            quat   rotation;

            position.x = data["position"][0].get<float>();
            position.y = data["position"][1].get<float>();
            position.z = data["position"][2].get<float>();
            scale.x    = data["scale"][0].get<float>();
            scale.y    = data["scale"][1].get<float>();
            scale.z    = data["scale"][2].get<float>();
            rotation.x = data["rotation"][0].get<float>();
            rotation.y = data["rotation"][1].get<float>();
            rotation.z = data["rotation"][2].get<float>();
            rotation.w = data["rotation"][3].get<float>();

            m_core = {position, scale, rotation};
        }

        inline void Inspect() override
        {
            auto [position, scale, rotation] = m_core.GetValue();
            float3 euler                     = glm::degrees(glm::eulerAngles(rotation));
            ImGui::DragFloat3("Position", reinterpret_cast<float*>(&position));
            ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&scale));
            ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&euler));
            rotation = glm::quat(glm::radians(euler));

            auto [old_position, old_scale, old_rotation] = m_core.GetValue();
            if (position != old_position || scale != old_scale || rotation != old_rotation)
            {
                m_core = {position, scale, rotation};
            }
        }

        /* ----------------------------- helpers ---------------------------- */

        // get transform matrix in S->R->T order
        inline float4x4 GetTransformMatrix() const
        {
            auto [position, scale, rotation] = m_core.GetValue();
            return glm::translate(glm::mat4_cast(rotation) * glm::scale(scale), position);
        }
    };
} // namespace GE