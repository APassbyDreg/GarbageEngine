#pragma once

#include "ComponentBase.h"

namespace GE
{
    enum class CameraType
    {
        Perspective,
        Orthographic
    };

    class CameraComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(CameraComponent);

        float      m_aspectRatio = 1.0f;
        float      m_horizonFov  = 54;
        float2     m_clip        = {1e-3, 1e3};
        CameraType m_type        = CameraType::Perspective;

        CameraComponent() {}

        inline json Serialize() const override
        {
            return {{"aspect", m_aspectRatio},
                    {"hfov", m_horizonFov},
                    {"clip", {m_clip.x, m_clip.y}},
                    {"type", (int)m_type}};
        }

        inline void Deserialize(const json& data) override
        {
            m_aspectRatio = data["aspect"].get<float>();
            m_horizonFov  = data["hfov"].get<float>();
            m_clip.x      = data["clip"][0].get<float>();
            m_clip.y      = data["clip"][1].get<float>();
            m_type        = __camera_id2type(data["type"].get<int>());
        }

        inline void Inspect() override
        {
            if (ImGui::CollapsingHeader("Perspective Camera"))
            {
                m_typeid                  = __camera_type2id(m_type);
                const char* type_names[2] = {"Perspective", "Orthographic"};
                const int   type_count    = 2;
                ImGui::Combo("Camera Type", &m_typeid, type_names, type_count);
                ImGui::DragFloat("Field of View (horizon)", &m_horizonFov);
                ImGui::DragFloat2("Z Clip (near -> far)", reinterpret_cast<float*>(&m_clip));
                if (m_clip.x > m_clip.y)
                {
                    float tmp = m_clip.x;
                    m_clip.x  = m_clip.y;
                    m_clip.y  = tmp;
                }
                m_type = __camera_id2type(m_typeid);
            }
        }

        /* ----------------------------- helpers ---------------------------- */

        inline int __camera_type2id(CameraType type)
        {
            switch (type)
            {
                case CameraType::Perspective:
                    return 0;
                case CameraType::Orthographic:
                    return 1;
            }
            return 0;
        }
        inline CameraType __camera_id2type(int id)
        {
            switch (id)
            {
                case 0:
                    return CameraType::Perspective;
                case 1:
                    return CameraType::Orthographic;
            };
            return CameraType::Perspective;
        }

        inline float4x4 GetProjectionMatrix() const
        {
            switch (m_type)
            {
                case CameraType::Perspective: {
                    return glm::perspective(glm::radians(m_horizonFov), 1.0f, m_clip.x, m_clip.y);
                }
                case CameraType::Orthographic: {
                    float half_width  = m_horizonFov * 0.5;
                    float half_height = half_width * m_aspectRatio;
                    return glm::ortho(-half_width, half_width, -half_height, half_height);
                }
            }
        }

    private:
        int m_typeid = 0;
    };

} // namespace GE