#ifndef COMMON_LIGHTDATA_GSH_
#define COMMON_LIGHTDATA_GSH_

struct PointLightData
{
    float3 intensity;
    float falloff_range;
    float3 position;
    float radius; // for small sphere light
};

struct SpotLightData
{
    float3 intensity;
    float falloff_range;
    float3 position;
    float inner_angle;
    float3 direction;
    float outer_angle;
    float3 up;
    float aspect; // width / height
};

struct DirectionLightData
{
    float3 intensity;
    float3 direction;
};

struct ProjectionLightData
{
    float3 intensity;
    float falloff_range;
    float3 position;
    float inner_angle;
    float3 direction;
    float outer_angle;
    float3 up;
};

[[vk::binding(0, 0)]]
cbuffer LightUniformBuffer
{
    uint point_light_count;
    uint spot_light_count;
    uint projection_light_count;
    uint direction_light_count;
}

[[vk::binding(1, 0)]]
StructuredBuffer<PointLightData> point_lights;
[[vk::binding(2, 0)]]
StructuredBuffer<SpotLightData> spot_lights;
[[vk::binding(3, 0)]]
StructuredBuffer<DirectionLightData> direction_lights;

#endif // COMMON_LIGHTDATA_GSH_