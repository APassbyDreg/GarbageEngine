#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "../Resource.h"

namespace GE
{
    namespace __Warper
    {
        enum ByteMagicNumber
        {
            UNKNOWN = 0,
        };
    }
    using ByteMagicNumber = __Warper::ByteMagicNumber;

    class ByteResource : public Resource<std::vector<byte>>
    {
    public:
        ByteResource(fs::path file, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::BYTE, file, use_cache, delayed_load)
        {}

        void Load() override;
        void Save() override;

    private:
        uint magic_number = 0;
    };
} // namespace GE