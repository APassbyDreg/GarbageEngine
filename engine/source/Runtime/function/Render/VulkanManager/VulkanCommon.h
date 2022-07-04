#pragma once

#include "GE_pch.h"

#include "Runtime/function/Log/LogSystem.h" // TODO: rethink layering
#include "vulkan/vulkan.h"

namespace GE
{

    inline std::string vkErrorString(VkResult errorCode)
    {
        switch (errorCode)
        {
#define STR(r) \
    case VK_##r: \
        return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }

    inline void __vk_check_fn(VkResult res)
    {
        if (res != VK_SUCCESS)
        {
            GE_CORE_CRITICAL("Vulkan call failed with code {0}", vkErrorString(res));
        }
    }
    inline bool __vk_check_fn_noexit(VkResult res)
    {
        if (res != VK_SUCCESS)
        {
            GE_CORE_ERROR("Vulkan call failed with code {0}", vkErrorString(res));
            return false;
        }
        return true;
    }

#define VK_CHECK(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        __vk_check_fn(__vk_check_res); \
    } while (0)

#define VK_CHECK_NOEXIT(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        __vk_check_fn_noexit(__vk_check_res); \
    } while (0)

#define VK_CHECK_NOEXIT_RESULT(call, result) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        result                  = __vk_check_fn_noexit(__vk_check_res); \
    } while (0)

} // namespace GE