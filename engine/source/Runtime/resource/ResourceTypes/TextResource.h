#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

namespace GE
{
    class GE_API TextResource : public Resource<std::string>
    {
    public:
        GE_RESOURCE_COMMON(TextResource, ResourceType::TEXT);

        inline void Invalid() override
        {
            if (m_valid)
            {
                m_valid = false;
                m_data  = "";
            }
        }
        inline void SaveData(const std::string& data) override
        {
            m_data  = data;
            m_valid = true;
            Save();
        }

    private:
    };
} // namespace GE