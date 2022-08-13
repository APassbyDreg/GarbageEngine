#pragma once

#include "GE_pch.h"

#include "core/math/math.h"

namespace GE
{

    struct PerspectiveCamera
    {
        float h_fov;
        float znear, zfar;

        float4x4 GetMatrixV() {}

        float4x4 GetMatrixP() {}
    };

    struct OrthographicCamera
    {
        float width;
        float znear, zfar;
    };

} // namespace GE