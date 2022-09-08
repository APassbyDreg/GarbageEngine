#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"

#include "GLFW/glfw3.h" // GLFW must be included after Vulkan
#include "VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Math/Math.h"

namespace GE
{
    class VulkanCore : public Singleton<VulkanCore>
    {
    public:
        VulkanCore();
        ~VulkanCore();

        void Init(GLFWwindow* window);

        void Destroy();

    public:
        /* ------------------------------- getters ------------------------------ */

        static inline VmaAllocator GetAllocator() { return GetInstance().ensure_ready().m_allocator; }

        static inline VkInstance       GetVkInstance() { return GetInstance().ensure_ready().m_instance; }
        static inline VkPhysicalDevice GetVkPhysicalDevice() { return GetInstance().ensure_ready().m_physicalDevice; }
        static inline VkDevice         GetVkDevice() { return GetInstance().ensure_ready().m_device; }
        static inline VkSurfaceKHR     GetVkSurface() { return GetInstance().ensure_ready().m_surface; }

        static inline VkQueue GetVkGraphicsQueue() { return GetInstance().ensure_ready().m_graphicsQueue; }
        static inline VkQueue GetVkPresentQueue() { return GetInstance().ensure_ready().m_presentQueue; }
        static inline VkQueue GetVkComputeQueue() { return GetInstance().ensure_ready().m_computeQueue; }
        static inline VkQueue GetVkTransferQueue() { return GetInstance().ensure_ready().m_transferQueue; }

        static inline uint32_t GetVkGraphicsQueueFamilyIndex()
        {
            return GetInstance().ensure_ready().m_graphicsQueueFamilyIndex;
        }
        static inline uint32_t GetVkPresentQueueFamilyIndex()
        {
            return GetInstance().ensure_ready().m_presentQueueFamilyIndex;
        }
        static inline uint32_t GetVkComputeQueueFamilyIndex(bool& support)
        {
            support = GetInstance().ensure_ready().m_supportCompute;
            return support ? GetInstance().ensure_ready().m_computeQueueFamilyIndex : 0;
        }
        static inline uint32_t GetVkTransferQueueFamilyIndex(bool& support)
        {
            support = GetInstance().ensure_ready().m_supportTransfer;
            return support ? GetInstance().ensure_ready().m_transferQueueFamilyIndex : 0;
        }

    private:
        inline VulkanCore& ensure_ready()
        {
            GE_CORE_ASSERT(m_ready, "VulkanCore is not initialized");
            return *this;
        }

        void init_vulkan(GLFWwindow* window);
        void destroy_vulkan();

        void init_vma();
        void destroy_vma();

    private:
        bool m_ready = false;

        /* ---------------------- vulkan core resources --------------------- */

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice         m_physicalDevice;
        VkDevice                 m_device;
        bool                     m_supportCompute, m_supportTransfer;
        VkQueue                  m_graphicsQueue, m_presentQueue;
        VkQueue                  m_computeQueue, m_transferQueue;
        uint32_t                 m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex;
        uint32_t                 m_computeQueueFamilyIndex, m_transferQueueFamilyIndex;
        VkSurfaceKHR             m_surface;

        /* -------------------------- vkb variables ------------------------- */
        vkb::Instance       m_vkbInstance;
        vkb::PhysicalDevice m_vkbPhysicalDevice;
        vkb::Device         m_vkbDevice;
        vkb::Swapchain      m_vkbSwapchain;

        /* -------------------------- vma allocator ------------------------- */
        VmaAllocator m_allocator;
    };
} // namespace GE

#define VKB_CHECK_RETURN(src, dest) \
    do \
    { \
        auto ret     = src; \
        bool success = ret.has_value(); \
        GE_CORE_ASSERT(success, "VKB failed when running {}", #src); \
        if (success) \
            dest = ret.value(); \
    } while (false)