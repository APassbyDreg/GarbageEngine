#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

#include "Runtime/function/Render/VulkanManager/DescriptorSetLayout.h"
#include <memory>

namespace GE
{
    struct CameraInfo;

    // todo better handle pad bytes
    struct CameraUniform
    {
        alignas(16) float4x4 world_to_view;
        alignas(16) float4x4 view_to_clip;
        alignas(16) float4x4 world_to_clip;
        alignas(16) float4x4 clip_to_view;
        alignas(16) float4x4 clip_to_world;
        alignas(16) float3 pos;     // with 4 pad bytes
        alignas(16) float3 forward; // with 4 pad bytes
        alignas(16) float3 up;      // with 4 pad bytes
        alignas(16) float3 right;   // with 4 pad bytes
        alignas(8) float2 clip;
        alignas(8) float2 inv_clip;

        static CameraUniform FromCameraInfo(CameraInfo& info);
    };

    // TODO: reference UE: Engine/Source/Runtime/Engine/Public/SceneView.h
    // align with gpu address
    struct ViewUniform
    {
        /* ----------------------------- camera ----------------------------- */
        alignas(16) CameraUniform curr_cam;
        alignas(16) CameraUniform prev_cam;
        /* ----------------------------- screen ----------------------------- */
        alignas(8) float2 resolution;
        alignas(8) float2 inv_resolution;
        /* ----------------------------- states ----------------------------- */
        alignas(4) float time;       // in seconds
        alignas(4) float delta_time; // in seconds
        alignas(4) uint frame_number;
        alignas(4) uint random;
        /* ---------------------------- settings ---------------------------- */
        alignas(4) uint debug_flag     = 0;
        alignas(4) uint has_prev_frame = 0;
        // float4 taa_param;
        

        /* ---------------------------- functions --------------------------- */
        static VkDescriptorSetLayout GetDescriptorSetLayout();

        static inline VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding()
        {
            return VkInit::GetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 0);
        }
    };
} // namespace GE