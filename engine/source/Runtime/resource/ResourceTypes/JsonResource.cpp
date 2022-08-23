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
            m_identifier = string2jsonId(m_data["type"].get<std::string>());
            m_loaded     = true;
        }
    }

    void JsonResource::Save()
    {
        m_data["GE_IDENTIFIER"] = jsonId2string(m_identifier);
        std::ofstream file(m_filePath.string());
        file << m_data;
        file.close();
    }
} // namespace GE