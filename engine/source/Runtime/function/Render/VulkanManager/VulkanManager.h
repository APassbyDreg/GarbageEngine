#pragma once

#include "GE_pch.h"

#include "VkCommon.h"

#include "GLFW/glfw3.h" // GLFW must be included after Vulkan
#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

#include "core/base/Singleton.h"
#include "core/math/math.h"

namespace GE
{
    class VulkanManager : public Singleton<VulkanManager>
    {
    public:
        VulkanManager();
        ~VulkanManager();

        void Init(GLFWwindow* window);

        void Destroy();

    public:
        /* ------------------------------- getters ------------------------------ */

        inline VkInstance       GetVkInstance() const { return m_instance; }
        inline VkPhysicalDevice GetVkPhysicalDevice() const { return m_physicalDevice; }
        inline VkDevice         GetVkDevice() const { return m_device; }
        inline VkSurfaceKHR     GetVkSurface() const { return m_surface; }
        inline VkQueue          GetVkGraphicsQueue() const { return m_graphicsQueue; }
        inline VkQueue          GetVkPresentQueue() const { return m_presentQueue; }
        inline VkQueue          GetVkComputeQueue() const { return m_computeQueue; }
        inline VkQueue          GetVkTransferQueue() const { return m_transferQueue; }
        inline uint32_t         GetVkGraphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }
        inline uint32_t         GetVkPresentQueueFamilyIndex() const { return m_presentQueueFamilyIndex; }
        inline uint32_t         GetVkComputeQueueFamilyIndex() const { return m_computeQueueFamilyIndex; }
        inline uint32_t         GetVkTransferQueueFamilyIndex() const { return m_transferQueueFamilyIndex; }

    private:
        void init_vulkan(GLFWwindow* window);
        void destroy_vulkan();

        void init_vma();
        void destroy_vma();

    private:
        bool m_initialized = false;

        /* ---------------------- vulkan core resources --------------------- */

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice         m_physicalDevice;
        VkDevice                 m_device;
        VkQueue                  m_graphicsQueue, m_presentQueue, m_computeQueue, m_transferQueue;
        uint32_t                 m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex, m_computeQueueFamilyIndex,
            m_transferQueueFamilyIndex;
        VkSurfaceKHR m_surface;

        /* ------------------- vulkan swapchain resources ------------------- */

        VkExtent2D               m_swapchainExtent;
        VkFormat                 m_swapchainFormat;
        VkSwapchainKHR           m_swapchain;
        std::vector<VkImage>     m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        /* -------------------------- vkb variables ------------------------- */
        vkb::Instance       m_vkbInstance;
        vkb::PhysicalDevice m_vkbPhysicalDevice;
        vkb::Device         m_vkbDevice;
        vkb::Swapchain      m_vkbSwapchain;

        /* -------------------------- vma allocator ------------------------- */
        VmaAllocator m_allocator;
    };
} // namespace GE