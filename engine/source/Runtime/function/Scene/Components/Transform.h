#pragma once

#include "ComponentBase.h"

namespace GE
{
    /**
     * We use left hand coordinate system:
     * - left: x+
     * - up: y+
     * - forward: z+
     */
    struct TransformComponentCore
    {
        float3 position = {0, 0, 0};
        float3 scale    = {1, 1, 1};
        quat   rotation = {1, 0, 0, 0};

        inline float3 Forward() { return rotation * float3(0, 0, 1); }
        inline float3 Up() { return rotation * float3(0, 1, 0); }
        inline float3 Right() { return rotation * float3(1, 0, 0); }

        inline bool operator==(TransformComponentCore&& rhs)
        {
            return position == rhs.position && scale == rhs.scale && rotation == rhs.rotation;
        }

        inline operator std::tuple<float3, float3, quat>() { return {position, scale, rotation}; }
    };

    class GE_API TransformComponent : public ComponentBase
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

        void Inspect() override;

        /* ----------------------------- helpers ---------------------------- */

        // get transform matrix in S->R->T order
        inline float4x4 GetTransformMatrix() const
        {
            auto&& [position, scale, rotation] = m_core.GetValue();
            return glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
        }

        inline float3 GetPosition() const { return m_core.GetValue().position; }
        inline float3 GetScale() const { return m_core.GetValue().scale; }
        inline quat   GetRotation() const { return m_core.GetValue().rotation; }
    };
} // namespace GE