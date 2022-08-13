#pragma once

#include "GE_pch.h"

#include "core/math/math.h"

namespace GE
{
    struct Transform
    {
        float3 position;
        float3 scale;
        quat   rotation;
    };
} // namespace GE