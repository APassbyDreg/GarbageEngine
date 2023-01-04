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
        float      aspectRatio = 1.0f;
        float      horizonFov  = 54;
        float2     clip        = {1e-3, 1e3};
        CameraType type        = CameraType::Perspective;

        bool operator==(CameraComponentCore&& rhs)
        {
            return aspectRatio == rhs.aspectRatio && horizonFov == rhs.horizonFov && clip == rhs.clip &&
                   type == rhs.type;
        }

        operator std::tuple<float, float, float2, CameraType>() { return {aspectRatio, horizonFov, clip, type}; }
    };

    class CameraComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(CameraComponent);

    public:
        inline json Serialize() const override
        {
            auto [aspect, fov, clip, cam_type] = m_core.GetValue();
            return {{"aspect", aspect}, {"hfov", fov}, {"clip", {clip.x, clip.y}}, {"type", (int)cam_type}};
        }

        inline void Deserialize(const json& data) override
        {
            float      aspect, fov;
            float2     clip;
            CameraType cam_type;

            aspect   = data["aspect"].get<float>();
            fov      = data["hfov"].get<float>();
            clip.x   = data["clip"][0].get<float>();
            clip.y   = data["clip"][1].get<float>();
            cam_type = __camera_id2type(data["type"].get<int>());

            m_core = {aspect, fov, clip, cam_type};
        }

        inline void Inspect() override
        {
            auto [aspect, fov, clip, cam_type] = m_core.GetValue();
            m_typeid                           = __camera_type2id(cam_type);
            const char* type_names[2]          = {"Perspective", "Orthographic"};
            const int   type_count             = 2;
            ImGui::Combo("Camera Type", &m_typeid, type_names, type_count);
            ImGui::DragFloat("Field of View (horizon)", &fov);
            ImGui::DragFloat2("Z Clip (near -> far)", reinterpret_cast<float*>(&clip));
            if (clip.x > clip.y)
            {
                float tmp = clip.x;
                clip.x    = clip.y;
                clip.y    = tmp;
            }
            cam_type = __camera_id2type(m_typeid);

            auto [old_aspect, old_fov, old_clip, old_cam_type] = m_core.GetValue();
            if (old_aspect != aspect || old_fov != fov || old_clip != clip || old_cam_type != cam_type)
            {
                m_core = {aspect, fov, clip, cam_type};
            }
        }

        inline float4x4 GetProjectionMatrix() const
        {
            auto [aspect, fov, clip, cam_type] = m_core.GetValue();
            switch (cam_type)
            {
                case CameraType::Perspective: {
                    return glm::perspective(glm::radians(fov), 1.0f, clip.x, clip.y);
                }
                case CameraType::Orthographic: {
                    float half_width  = fov * 0.5;
                    float half_height = half_width * aspect;
                    return glm::ortho(-half_width, half_width, -half_height, half_height);
                }
            }
            return float4x4(1.0f);
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