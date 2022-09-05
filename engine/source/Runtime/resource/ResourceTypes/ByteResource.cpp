#include "ByteResource.h"

#include "Runtime/core/Packing.h"

namespace GE
{
    void ByteResource::Load()
    {
        if (fs::exists(m_filePath))
        {
            // load basic data
            uint64        cmp_size, full_size;
            char*         cmp_data;
            std::ifstream file(m_filePath.string(), std::ios::binary);
            file.read((char*)&m_magicnumber, sizeof(uint64));
            file.read((char*)&full_size, sizeof(uint64));
            file.read((char*)&cmp_size, sizeof(uint64));

            // alloc memory
            cmp_data = new char[cmp_size];
            file.read((char*)cmp_data, cmp_size);

            // decompress
            byte* tmp;
            if (full_size > cmp_size)
            {
                tmp = new byte[full_size + 10];
                Packing::DecompressData((uchar*)cmp_data, cmp_size, (uchar**)&tmp, full_size);
            }
            else if (full_size == cmp_size)
            {
                tmp = (byte*)cmp_data;
            }
            else
            {
                GE_CORE_ERROR("ByteResource::Load: full_size < cmp_size");
                return;
            }
            m_data = std::vector<byte>(tmp, tmp + full_size);

            // cleanup
            file.close();
            delete[] cmp_data;

            m_valid = true;
        }
    }

    void ByteResource::Save()
    {
        GE_CORE_ASSERT(m_valid, "[ByteResource::Save] Trying to save invalid resource to {}", m_filePath.string());

        // compress
        uint64 full_size = m_data.size() * sizeof(byte);
        uint64 cmp_size;
        uchar* cmp_data;
        Packing::CompressData((uchar*)m_data.data(), full_size, &cmp_data, cmp_size);

        // write
        std::ofstream file(m_filePath.string(), std::ios::binary);
        file.write((char*)&m_magicnumber, sizeof(uint64));
        file.write((char*)&full_size, sizeof(uint64));
        file.write((char*)&cmp_size, sizeof(uint64));
        file.write((char*)cmp_data, cmp_size);
        file.close();
    }
} // namespace GE