#ifndef COMMON_VIEWUNIFORM_GSH_
#define COMMON_VIEWUNIFORM_GSH_

#include "Transforms.gsh"

struct CameraUniform
{
    float4x4 world_to_view;
    float4x4 view_to_clip;
    float4x4 world_to_clip;
    float4x4 clip_to_view;
    float4x4 clip_to_world;
    float3   pos;
    float3   forward;
    float3   up;
    float3   right;
    float2   clip;
    float2   inv_clip;
    float2x2 to_linear_depth;
    float2x2 to_device_depth;
};

// TODO: reference UE: Engine/Source/Runtime/Engine/Public/SceneView.h
struct ViewUniform
{
    /* ----------------------------- states ----------------------------- */
    float time;
    float delta_time;
    uint  frame_number;
    uint  random;
    /* ----------------------------- screen ----------------------------- */
    float2 resolution;
    float2 inv_resolution;
    /* ----------------------------- camera ----------------------------- */
    CameraUniform curr_cam;
    CameraUniform prev_cam;
    /* ---------------------------- settings ---------------------------- */
    uint debug_flag;
    // float4 taa_param;
};

[[vk::binding(0, 0)]]
cbuffer ViewUniformBuffer
{
    ViewUniform view;
}

float4 TransformVertex(float3 vposWS)
{
    return mul(view.curr_cam.world_to_clip, float4(vposWS, 1.0));
}

float3 TransformWorldSpaceToViewSpace(float3 posWS)
{
    return HomogeneousTransform(posWS, view.curr_cam.world_to_view);
}

float3 TransformWorldSpaceToClipSpace(float3 posWS)
{
    return HomogeneousTransform(posWS, view.curr_cam.world_to_clip);
}

float3 TransformViewSpaceToClipSpace(float3 posVS)
{
    return HomogeneousTransform(posVS, view.curr_cam.view_to_clip);
}

float3 TransformClipSpaceToViewSpace(float3 posCS)
{
    return HomogeneousTransform(posCS, view.curr_cam.clip_to_view);
}

float3 TransformClipSpaceToWorldSpace(float3 posCS)
{
    return HomogeneousTransform(posCS, view.curr_cam.clip_to_world);
}

float DeviceToLinearDepth(float device_depth)
{
    return HomogeneousTransform(device_depth, view.curr_cam.to_linear_depth);
}

float LinearToDeviceDepth(float linear_depth)
{
    return HomogeneousTransform(linear_depth, view.curr_cam.to_device_depth);
}

#endif // COMMON_PIPELINE_VIEWUNIFORM_GSH_