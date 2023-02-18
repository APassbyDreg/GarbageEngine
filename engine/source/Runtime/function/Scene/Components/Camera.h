#pragma once

#include "ComponentBase.h"

namespace GE
{
    enum class CameraType
    {
        Perspective,
        Orthographic
    };

    struct CameraComponentCore
    {
        float      fov         = 54;
        float2     clip        = {1e-3, 1e3};
        CameraType type        = CameraType::Perspective;
        bool       active      = false;

        float4x4 GetProjectionMatrix(float aspect) const
        {
            switch (type)
            {
                case CameraType::Perspective: {
                    return glm::perspective(glm::radians(fov), aspect, clip.x, clip.y);
                }
                case CameraType::Orthographic: {
                    float half_width  = Max<float>(fov, 0.01) * 0.5;
                    float half_height = half_width / aspect;
                    return glm::ortho(-half_width, half_width, -half_height, half_height, clip.x, clip.y);
                }
            }
            return float4x4(1.0f);
        }

        bool operator==(CameraComponentCore&& rhs)
        {
            return active == rhs.active && fov == rhs.fov && clip == rhs.clip && type == rhs.type;
        }
    };

    class GE_API CameraComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(CameraComponent);

    public:
        inline json Serialize() const override
        {
            auto&& [fov, clip, cam_type, active] = m_core.GetValue();
            return {{"hfov", fov}, {"clip", {clip.x, clip.y}}, {"type", (int)cam_type}, {"active", active}};
        }

        inline void Deserialize(const json& data) override
        {
            float      fov;
            float2     clip;
            CameraType cam_type;
            bool       active;

            fov      = data["hfov"].get<float>();
            clip.x   = data["clip"][0].get<float>();
            clip.y   = data["clip"][1].get<float>();
            cam_type = __camera_id2type(data["type"].get<int>());
            active   = data["active"].get<bool>();

            m_core = {fov, clip, cam_type, active};
        }

        inline void Inspect() override
        {
            auto [fov, clip, cam_type, active] = m_core.GetValue();
            m_typeid                           = __camera_type2id(cam_type);
            const char* type_names[2]          = {"Perspective", "Orthographic"};
            const int   type_count             = 2;
            ImGui::Combo(LABEL_WITH_NAME("Camera Type"), &m_typeid, type_names, type_count);
            const char* fov_name = cam_type == CameraType::Perspective ? "Field of View (Y)" : "Width";
            ImGui::DragFloat(LABEL_WITH_NAME(fov_name), &fov, 0.1, 0.01, 150.00);
            ImGui::DragFloat2(LABEL_WITH_NAME("Z Clip (near -> far)"), reinterpret_cast<float*>(&clip), 0.1, 0.1);
            if (clip.x > clip.y)
            {
                float tmp = clip.x;
                clip.x    = clip.y;
                clip.y    = tmp;
            }
            cam_type = __camera_id2type(m_typeid);
            ImGui::Checkbox(LABEL_WITH_NAME("Is Active"), &active);

            auto [old_fov, old_clip, old_cam_type, old_active] = m_core.GetValue();
            if (old_fov != fov || old_clip != clip || old_cam_type != cam_type || old_active != active)
            {
                m_core = {fov, clip, cam_type, active};
            }
        }

        inline float4x4 GetProjectionMatrix(float aspect) const
        {
            return m_core.GetValue().GetProjectionMatrix(aspect);
        }

    private:
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

    private:
        int m_typeid = 0;
    };

} // namespace GE