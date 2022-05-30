#pragma once

#include "GE_pch.h"

#include "function/Log/LogSystem.h" // TODO: rethink layering
#include "vulkan/vulkan.h"

namespace GE
{
    inline void __vk_check_fn(VkResult res)
    {
        if (res != VK_SUCCESS)
        {
            GE_CORE_CRITICAL("Vulkan call failed with code {0}", res);
        }
    }
    inline void __vk_check_fn_noexit(VkResult res)
    {
        if (res != VK_SUCCESS)
        {
            GE_CORE_ERROR("Vulkan call failed with code {0}", res);
        }
    }

#define VK_CHECK(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        __vk_check_fn_noexit(__vk_check_res); \
    } while (0)

#define VK_CHECK_CRITICAL(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        __vk_check_fn(__vk_check_res); \
    } while (0)

} // namespace GE