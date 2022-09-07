#pragma once

#include "ComponentBase.h"

namespace GE
{
    class TagComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(TagComponent);

        std::string m_name  = "unnamed entity";
        int         m_layer = 0, m_tag = 0;

        TagComponent(std::string name = "unnamed entity", int layer = 0, int tag = 0) :
            m_name(name), m_layer(layer), m_tag(tag)
        {}

        inline json Serialize() const override { return {{"name", m_name}, {"layer", m_layer}, {"tag", m_tag}}; }

        inline void Deserialize(const json& data) override
        {
            m_name  = data["name"].get<std::string>();
            m_layer = data["layer"].get<int>();
            m_tag   = data["tag"].get<int>();
        }

        inline void Inspect() override
        {
            strcpy(m_nameBuffer, m_name.c_str());
            ImGui::InputText("Name", m_nameBuffer, 256);
            ImGui::InputInt("Layer", &m_layer);
            ImGui::InputInt("Tag", &m_tag);
            m_name = m_nameBuffer;
        }

    private:
        char m_nameBuffer[256] = {};
    };
} // namespace GE