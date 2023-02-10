#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "Runtime/function/Render/VulkanManager/DescriptorSetLayout.h"
#include <memory>

namespace GE
{
    struct CameraInfo;

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

        static CameraUniform FromCameraInfo(CameraInfo& info);
    };

    // TODO: reference UE: Engine/Source/Runtime/Engine/Public/SceneView.h
    struct ViewUniform
    {
        /* ----------------------------- states ----------------------------- */
        float time;       // in seconds
        float delta_time; // in seconds
        uint  frame_number;
        uint  random;
        /* ----------------------------- screen ----------------------------- */
        float2 resolution;
        float2 inv_resolution;
        /* ----------------------------- camera ----------------------------- */
        CameraUniform curr_cam;
        CameraUniform prev_cam;
        /* ---------------------------- settings ---------------------------- */
        uint debug_flag     = 0;
        uint has_prev_frame = 0;
        // float4 taa_param;
        

        /* ---------------------------- functions --------------------------- */
        static VkDescriptorSetLayout GetDescriptorSetLayout();
    };
} // namespace GE