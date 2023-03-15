#pragma once

#include "GE_pch.h"

namespace GE
{
    enum LightType
    {
        LIGHT_TYPE_POINT      = 0,
        LIGHT_TYPE_SPOT       = 1,
        LIGHT_TTPE_PROJECTION = 2,
        LIGHT_TYPE_DIRECTION  = 3,
        LIGHT_TYPE_UNKNOWN    = 0xff,
    };
} // namespace GE