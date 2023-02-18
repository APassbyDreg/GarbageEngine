#pragma once

#include "ComponentBase.h"

namespace GE
{
    struct TagComponentCore
    {
        std::string name  = "unnamed entity";
        int         layer = 0, tag = 0;

        bool operator==(TagComponentCore&& rhs) { return name == rhs.name && layer == rhs.layer && tag == rhs.tag; }
             operator std::tuple<std::string, int, int>() { return {name, layer, tag}; }
    };

    class GE_API TagComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(TagComponent);

    public:
        TagComponent(std::shared_ptr<Entity> e, std::string name, int layer, int tag) : ComponentBase(e)
        {
            m_core = {name, layer, tag};
        }

        inline std::string GetTagName() const { return m_core.GetValue().name; }
        inline int         GetTagID() const { return m_core.GetValue().tag; }
        inline int         GetLayerID() const { return m_core.GetValue().layer; }

        inline json Serialize() const override
        {
            auto [name, layer, tag] = m_core.GetValue();
            return {{"name", name}, {"layer", layer}, {"tag", tag}};
        }

        inline void Deserialize(const json& data) override
        {
            std::string name;
            int         layer, tag;
            name   = data["name"].get<std::string>();
            layer  = data["layer"].get<int>();
            tag    = data["tag"].get<int>();
            m_core = {name, layer, tag};
        }

        inline void Inspect() override
        {
            auto [name, layer, tag] = m_core.GetValue();
            char nameBuffer[256]    = {};
            strcpy(nameBuffer, name.c_str());
            ImGui::InputText(LABEL_WITH_NAME("Name"), nameBuffer, 256);
            ImGui::InputInt(LABEL_WITH_NAME("Layer"), &layer);
            ImGui::InputInt(LABEL_WITH_NAME("Tag"), &tag);
            name   = nameBuffer;

            auto [old_name, old_layer, old_tag] = m_core.GetValue();
            if (name != old_name || layer != old_layer || tag != old_tag)
            {
                m_core = {name, layer, tag};
            }
        }
    };
} // namespace GE