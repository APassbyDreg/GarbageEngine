#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/json.h"
#include "Runtime/core/math/math.h"

namespace GE
{
    enum class CameraType
    {
        Perspective,
        Orthographic
    };

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

    class CameraComponent
    {
    public:
        float      m_horizonFov = 10;
        float2     m_clip       = {1e-3, 1e3};
        CameraType m_type       = CameraType::Perspective;

        CameraComponent() {}

        inline json Serialize() const
        {
            return {{"hfov", m_horizonFov}, {"clip", {m_clip.x, m_clip.y}}, {"type", (int)m_type}};
        }

        void Inspect()
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

    private:
        int m_typeid = 0;
    };

} // namespace GE