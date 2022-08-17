#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/json.h"
#include "Runtime/core/math/math.h"

namespace GE
{
    class TagComponent
    {
    public:
        std::string m_name  = "unnamed entity";
        int         m_layer = 0, m_tag = 0;

        TagComponent(std::string name = "unnamed entity", int layer = 0, int tag = 0) :
            m_name(name), m_layer(layer), m_tag(tag)
        {}

        inline json Serialize() const { return {{"name", m_name}, {"layer", m_layer}, {"tag", m_tag}}; }

        inline void Inspect()
        {
            if (ImGui::CollapsingHeader("Tag"))
            {
                strcpy(m_nameBuffer, m_name.c_str());
                ImGui::InputText("Name", m_nameBuffer, 256);
                ImGui::InputInt("Layer", &m_layer);
                ImGui::InputInt("Tag", &m_tag);
                m_name = std::string(m_nameBuffer);
            }
        }

    private:
        char m_nameBuffer[256] = {};
    };
} // namespace GE