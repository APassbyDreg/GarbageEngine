#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/json.h"
#include "Runtime/core/math/math.h"

namespace GE
{
    class TransformComponent
    {
    public:
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

        // get transform matrix in S->R->T order
        inline float4x4 GetTransformMatrix() const
        {
            return glm::translate(glm::mat4_cast(m_rotation) * glm::scale(m_scale), m_position);
        }

        inline json Serialize() const
        {
            return {{"position", {m_position.x, m_position.y, m_position.z}},
                    {"scale", {m_scale.x, m_scale.y, m_scale.z}},
                    {"rotation", {m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w}}};
        }

        inline void Inspect()
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                ImGui::DragFloat3("Position", reinterpret_cast<float*>(&m_position));
                ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&m_scale));
                ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&m_eulerAngles));
                m_rotation = glm::quat(glm::radians(m_eulerAngles));
            }
        }

    private:
        float3 m_eulerAngles = {0, 0, 0};
    };
} // namespace GE