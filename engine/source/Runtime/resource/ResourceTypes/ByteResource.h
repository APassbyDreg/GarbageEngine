#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "../ResourceBase.h"

namespace GE
{
    namespace ByteResourceMagicNumber
    {
        const uint64 UNKNOWN = 0;
        const uint64 MESH    = 1;
    } // namespace ByteResourceMagicNumber

    class GE_API ByteResource : public Resource<std::vector<byte>>
    {
    public:
        GE_RESOURCE_COMMON(ByteResource, ResourceType::BYTE);

        ByteResource(fs::path file,
                     uint64   magic_number = 0,
                     bool     init         = false,
                     bool     use_cache    = false,
                     bool     delayed_load = false) :
            Resource(ResourceType::BYTE, file, init, use_cache, delayed_load),
            m_magicnumber(magic_number)
        {
            if (!delayed_load)
                Load();
            if (init && !m_valid)
                Initialize();
        }

        void Invalid() override
        {
            if (m_valid)
            {
                m_valid = false;
                m_data  = {};
            }
        }
        void SaveData(const std::vector<byte>& data) override
        {
            m_data  = data;
            m_valid = true;
            Save();
        }

        /*
         * 0 - UNKNOWN
         * 1 - MESH
         */
        uint64 m_magicnumber = 0;
    };
} // namespace GE