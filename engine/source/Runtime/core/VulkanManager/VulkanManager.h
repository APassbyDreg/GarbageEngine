#pragma once

#include "GE_pch.h"

#include "VkBootstrap.h"
// GLFW must be included after Vulkan
#include "GLFW/glfw3.h"

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

        void RecreateSwapchain(int2 extent);

    private:
        void init_vulkan(GLFWwindow* window);
        void destroy_vulkan();

        void init_swapchain();
        void destroy_swapchain();

    private:
        bool m_initialized = false;

        /* ---------------------- vulkan core resources --------------------- */

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice         m_physicalDevice;
        VkDevice                 m_device;
        VkSurfaceKHR             m_surface;

        /* ------------------- vulkan swapchain resources ------------------- */

        int2                     m_swapchainExtent;
        VkSwapchainKHR           m_swapchain;
        std::vector<VkImage>     m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        /* -------------------------- vkb variables ------------------------- */
        vkb::Instance  m_vkbInstance;
        vkb::Device    m_vkbDevice;
        vkb::Swapchain m_vkbSwapchain;
    };
} // namespace GE