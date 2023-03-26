#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct PhongParams
    {
        alignas(16) float3 kd;
        alignas(16) float3 ks;
        alignas(16) float3 ka;
        float ns;
    };
} // namespace GE