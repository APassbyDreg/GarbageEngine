#pragma once

#include "Transform.h"
#include "Camera.h"

#include "../Entity.h"

namespace GE
{
    void TransformComponent::Inspect()
    {
        auto [position, scale, rotation] = m_core.GetValue();
        float3 euler                     = glm::degrees(glm::eulerAngles(rotation));
        ImGui::DragFloat3(LABEL_WITH_NAME("Position"), reinterpret_cast<float*>(&position), 0.1);
        if (!m_entity->HasComponent<CameraComponent>())
        {
            ImGui::DragFloat3(LABEL_WITH_NAME("Scale"), reinterpret_cast<float*>(&scale), 0.1);
        }
        ImGui::DragFloat3(LABEL_WITH_NAME("Rotation"), reinterpret_cast<float*>(&euler), 0.1);
        rotation = glm::quat(glm::radians(euler));

        auto [old_position, old_scale, old_rotation] = m_core.GetValue();
        if (position != old_position || scale != old_scale || rotation != old_rotation)
        {
            m_core = {position, scale, rotation};
        }
    }
} // namespace GE