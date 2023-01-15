#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

#include "Runtime/core/Json.h"

namespace GE
{
    enum class JsonIdentifier
    {
        SCENE_DESCRIPTION,
        INSTANCED_COMPONENT,
        MATERIAL,
        MESH,
        UNKNOWN
    };

    class GE_API JsonResource : public Resource<json>
    {
    public:
        JsonResource(fs::path file, JsonIdentifier type, bool delayed_load = false) :
            Resource(ResourceType::JSON, file, false, delayed_load), m_identifier(type)
        {}

        void Load() override;
        void Save() override;

        void Invalid() override
        {
            if (m_valid)
            {
                m_valid = false;
                m_data  = {};
            }
        }
        void SaveData(const json& data) override
        {
            m_data  = data;
            m_valid = true;
            Save();
        }

    private:
        inline std::string jsonId2string(JsonIdentifier id)
        {
            switch (id)
            {
                case JsonIdentifier::SCENE_DESCRIPTION:
                    return "SCENE_DESCRIPTION";
                case JsonIdentifier::INSTANCED_COMPONENT:
                    return "INSTANCED_COMPONENT";
                case JsonIdentifier::MATERIAL:
                    return "MATERIAL";
                case JsonIdentifier::MESH:
                    return "MESH";
                default:
                    return "UNKNOWN";
            }
        }
        inline JsonIdentifier string2jsonId(std::string id)
        {
            if (id == "SCENE_DESCRIPTION")
                return JsonIdentifier::SCENE_DESCRIPTION;
            else if (id == "INSTANCED_COMPONENT")
                return JsonIdentifier::INSTANCED_COMPONENT;
            else if (id == "MATERIAL")
                return JsonIdentifier::MATERIAL;
            else if (id == "MESH")
                return JsonIdentifier::MESH;
            else
                return JsonIdentifier::UNKNOWN;
        }

        JsonIdentifier m_identifier;
    };
} // namespace GE