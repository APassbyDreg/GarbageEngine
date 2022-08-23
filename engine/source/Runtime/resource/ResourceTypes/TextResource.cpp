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
            m_data   = buffer.str();
            m_loaded = true;
            file.close();
        }
    }

    void TextResource::Save()
    {
        std::ofstream file(m_filePath.string());
        file << m_data;
        file.close();
    }
} // namespace GE