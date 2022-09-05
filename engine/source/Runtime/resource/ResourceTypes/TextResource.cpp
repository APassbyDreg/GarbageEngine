#include "TextResource.h"

namespace GE
{
    void TextResource::Load()
    {
        if (fs::exists(m_filePath))
        {
            std::ifstream     file(m_filePath.string());
            std::stringstream buffer;
            buffer << file.rdbuf();
            m_data = buffer.str();
            file.close();

            m_valid = true;
        }
    }

    void TextResource::Save()
    {
        GE_CORE_ASSERT(m_valid, "[TextResource::Save] Trying to save invalid resource to {}", m_filePath.string());

        std::ofstream file(m_filePath.string());
        file << m_data;
        file.close();
    }
} // namespace GE