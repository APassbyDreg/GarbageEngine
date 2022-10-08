#include "JsonResource.h"

namespace GE
{
    void JsonResource::Load()
    {
        if (fs::exists(m_filePath))
        {
            std::ifstream file(m_filePath.string());
            file >> m_data;
            file.close();
            m_identifier = string2jsonId(m_data["GE_IDENTIFIER"].get<std::string>());
            m_valid      = true;
        }
    }

    void JsonResource::Save()
    {
        GE_CORE_ASSERT(m_valid, "[JsonResource::Save] Trying to save invalid resource to {}", m_filePath.string());

        m_data["GE_IDENTIFIER"] = jsonId2string(m_identifier);
        std::ofstream file(m_filePath.string());
        file << m_data;
        file.close();
    }
} // namespace GE