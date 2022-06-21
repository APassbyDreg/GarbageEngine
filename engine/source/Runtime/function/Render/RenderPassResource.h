#pragma once

#include "GE_pch.h"

#include "Runtime/core/math/math.h"

namespace GE
{
    enum class RenderPassResourceType
    {
        RENDER_PASS_RESOURCE_TYPE_UNKNOWN,
        RENDER_PASS_RESOURCE_TYPE_TEXTURE,
        RENDER_PASS_RESOURCE_TYPE_BUFFER,
    };

    class RenderPassResource
    {
    public:
        RenderPassResourceType type;
        uint                   dim;
        std::vector<uint>      size;
    };

    class RenderPassResourceList
    {};
} // namespace GE