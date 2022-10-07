#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

namespace GE
{
    class GE_API TextResource : public Resource<std::string>
    {
    public:
        TextResource(fs::path file, bool use_cache = false, bool delayed_load = false) :
            Resource(ResourceType::TEXT, file, use_cache, delayed_load)
        {}

        void Load() override;
        void Save() override;

    private:
    };
} // namespace GE