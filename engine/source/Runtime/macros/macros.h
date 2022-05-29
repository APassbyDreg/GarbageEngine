#pragma once

// TODO: move things into dedicate files

/* ----------------------------------- API ---------------------------------- */
#if defined(_WIN32)
#ifdef GE_BUILD_RUNTIME
#define GE_API __declspec(dllexport)
#else
#define GE_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#define GE_API __attribute__((visibility("default")))
#else
#define GE_API
#endif

/* ------------------------------- bit fields ------------------------------- */
#define BIT(x) (1 << x)

/* ------------------------------ vulkan checks ----------------------------- */
#ifdef GE_DEBUG
#define VK_ERROR_MSG(call, res) \
    "Vulkan call [{0}] @ file [{1} : {2}] failed with code {3}\n", #call, __FILE__, __LINE__, res
#else
#define VK_ERROR_MSG(call, res) "Vulkan call [{0}] failed with code {1}\n", #call, res
#endif

#define VK_CHECK(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        if (__vk_check_res != VK_SUCCESS) \
        { \
            GE_CORE_ERROR(VK_ERROR_MSG(call, __vk_check_res)); \
        } \
    } while (0)

#define VK_CHECK_CRITICAL(call) \
    do \
    { \
        VkResult __vk_check_res = (call); \
        if (__vk_check_res != VK_SUCCESS) \
        { \
            GE_CORE_CRITICAL(VK_ERROR_MSG(call, __vk_check_res)); \
        } \
    } while (0)