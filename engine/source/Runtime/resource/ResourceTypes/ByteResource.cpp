#include "ByteResource.h"

namespace GE
{
    void ByteResource::Load()
    {
        if (fs::exists(m_filePath))
        {
            size_t        filesize = fs::file_size(m_filePath);
            std::ifstream file(m_filePath.string(), std::ios::in | std::ios::binary);

            file.seekg(0, std::ios::beg);
            m_data = std::vector<byte>(filesize);
            m_data.insert(m_data.begin(), std::istream_iterator<byte>(file), std::istream_iterator<byte>());

            file.close();
            m_loaded = true;
        }
    }

    void ByteResource::Save()
    {
        std::ofstream file(m_filePath.string(), std::ios::out | std::ios::binary);
        file.write((char*)m_data.data(), m_data.size());
        file.close();
    }
} // namespace GE