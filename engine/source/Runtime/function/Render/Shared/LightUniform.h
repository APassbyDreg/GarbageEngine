#pragma once

#include "GE_pch.h"

#include "../VulkanManager/VulkanCreateInfoBuilder.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    // NOTE: this file should be aligned with shaders/Common/LightData.gsh
    struct LightUniform
    {
        alignas(4) uint num_point_lights;
        alignas(4) uint num_spot_lights;
        alignas(4) uint num_direction_lights;
        alignas(4) uint num_projection_lights;
    };

    struct PointLightData
    {
        alignas(16) float3 intensity;
        alignas(4) float falloff_range;
        alignas(16) float3 position;
        alignas(4) float radius; // for small sphere light
    };

    struct SpotLightData
    {
        alignas(16) float3 intensity;
        alignas(4) float falloff_range;
        alignas(16) float3 position;
        alignas(4) float inner_angle;
        alignas(16) float3 direction;
        alignas(4) float outer_angle;
        alignas(16) float3 up;
        alignas(4) float aspect; // width / height
    };

    struct ProjectionLightData
    {
        alignas(16) float3 intensity;
        alignas(4) float falloff_range;
        alignas(16) float3 position;
        alignas(4) float inner_angle;
        alignas(16) float3 direction;
        alignas(4) float outer_angle;
        alignas(16) float3 up;
    };

    struct DirectionLightData
    {
        alignas(16) float3 intensity;
        alignas(16) float3 direction;
    };
} // namespace GE