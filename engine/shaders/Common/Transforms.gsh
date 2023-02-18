#ifndef COMMON_TRANSFORMS_GSH_
#define COMMON_TRANSFORMS_GSH_

float3 HomogeneousTransform(float3 vec, float4x4 mat)
{
    float4 vec4 = float4(vec, 1);
    vec4        = mul(mat, vec4);
    return vec4.xyz / vec4.w;
}

float2 HomogeneousTransform(float2 vec, float3x3 mat)
{
    float3 vec3 = float3(vec, 1);
    vec3        = mul(mat, vec3);
    return vec3.xy / vec3.z;
}

float HomogeneousTransform(float vec, float2x2 mat)
{
    float2 vec2 = float2(vec, 1);
    vec2        = mul(mat, vec2);
    return vec2.x / vec2.y;
}

float3 HomogeneousTransformVector(float3 vec, float4x4 mat)
{
    float4 vec4 = float4(vec, 1);
    vec4        = mul(mat, vec4);
    return normalize(vec4.xyz);
}

float2 HomogeneousTransformVector(float2 vec, float3x3 mat)
{
    float3 vec3 = float3(vec, 1);
    vec3        = mul(mat, vec3);
    return normalize(vec3.xy);
}

#endif // COMMON_TRANSFORMS_GSH_