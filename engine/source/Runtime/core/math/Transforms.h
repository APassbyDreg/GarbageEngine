#pragma once

#include "Math.h"

namespace GE
{
    namespace Math
    {
        inline float3 HomogeneousTransform(float3 vec, float4x4 mat)
        {
            float4 vec4 = float4(vec, 1);
            vec4        = vec4 * mat;
            return float3(vec4) / vec4.w;
        }

        inline float2 HomogeneousTransform(float2 vec, float3x3 mat)
        {
            float3 vec3 = float3(vec, 1);
            vec3        = vec3 * mat;
            return float2(vec3) / vec3.z;
        }
    } // namespace Math
} // namespace GE