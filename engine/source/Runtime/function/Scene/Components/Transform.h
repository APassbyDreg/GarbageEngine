#pragma once

#include "ComponentBase.h"

namespace GE
{
    class TransformComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(TransformComponent);

        float3 m_position = {0, 0, 0};
        float3 m_scale    = {1, 1, 1};
        quat   m_rotation = {1, 0, 0, 0};

        TransformComponent() {}
        TransformComponent(float3 pos, float3 scale, float3 rot) :
            m_position(pos), m_scale(scale), m_rotation(rot), m_eulerAngles(rot)
        {}
        TransformComponent(float3 pos, float3 scale, quat rot) : m_position(pos), m_scale(scale), m_rotation(rot)
        {
            m_eulerAngles = glm::degrees(glm::eulerAngles(m_rotation));
        }

        inline json Serialize() const override
        {
            return {{"position", {m_position.x, m_position.y, m_position.z}},
                    {"scale", {m_scale.x, m_scale.y, m_scale.z}},
                    {"rotation", {m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w}}};
        }

        inline void Deserialize(const json& data) override
        {
            m_position.x  = data["position"][0].get<float>();
            m_position.y  = data["position"][1].get<float>();
            m_position.z  = data["position"][2].get<float>();
            m_scale.x     = data["scale"][0].get<float>();
            m_scale.y     = data["scale"][1].get<float>();
            m_scale.z     = data["scale"][2].get<float>();
            m_rotation.x  = data["rotation"][0].get<float>();
            m_rotation.y  = data["rotation"][1].get<float>();
            m_rotation.z  = data["rotation"][2].get<float>();
            m_rotation.w  = data["rotation"][3].get<float>();
            m_eulerAngles = glm::degrees(glm::eulerAngles(m_rotation));
        }

        inline void Inspect() override
        {
            ImGui::DragFloat3("Position", reinterpret_cast<float*>(&m_position));
            ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&m_scale));
            ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&m_eulerAngles));
            m_rotation = glm::quat(glm::radians(m_eulerAngles));
        }

        /* ----------------------------- helpers ---------------------------- */

        // get transform matrix in S->R->T order
        inline float4x4 GetTransformMatrix() const
        {
            return glm::translate(glm::mat4_cast(m_rotation) * glm::scale(m_scale), m_position);
        }

    private:
        float3 m_eulerAngles = {0, 0, 0};
    };
} // namespace GE