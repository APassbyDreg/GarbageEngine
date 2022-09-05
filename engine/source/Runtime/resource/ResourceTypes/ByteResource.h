#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "../Resource.h"

namespace GE
{
    namespace ByteResourceMagicNumber
    {
        const uint64 UNKNOWN = 0;
        const uint64 MESH    = 1;
    } // namespace ByteResourceMagicNumber

    class ByteResource : public Resource<std::vector<byte>>
    {
    public:
        ByteResource(fs::path file, uint64 magic_number = 0, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::BYTE, file, use_cache, delayed_load), m_magicnumber(magic_number)
        {}

        void Load() override;
        void Save() override;

        /*
         * 0 - UNKNOWN
         * 1 - MESH
         */
        uint64 m_magicnumber = 0;
    };
} // namespace GE