#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

namespace GE
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain() {}
        VulkanSwapchain(VkExtent2D extent);
        ~VulkanSwapchain();

        void Resize(VkExtent2D extent);

        inline VkImage GetImage(uint index)
        {
            GE_CORE_ASSERT(m_ready, "swapchain not ready");
            GE_CORE_ASSERT(index < m_imageCount, "index out of range");
            return m_swapchainImages[index];
        }
        inline VkImageView GetImageView(uint index)
        {
            GE_CORE_ASSERT(m_ready, "swapchain not ready");
            GE_CORE_ASSERT(index < m_imageCount, "index out of range");
            return m_swapchainImageViews[index];
        }
        inline int GetNextIndex()
        {
            GE_CORE_ASSERT(m_ready, "swapchain not ready");
            uint idx;
            bool success;
            VK_CHECK_NOEXIT_RESULT(vkAcquireNextImageKHR(VulkanCore::GetVkDevice(),
                                                         m_swapchain,
                                                         1000,
                                                         m_imageAvailableSemaphores[m_currentIndex],
                                                         VK_NULL_HANDLE,
                                                         &idx),
                                   success);
            m_currentIndex = (m_currentIndex + 1) % m_imageCount;
            return success ? idx : -1;
        }

    private:
        void __create_swapchain();
        void __destroy_swapchain();

    private:
        bool m_ready = false;

        VkExtent2D               m_swapchainExtent;
        VkFormat                 m_swapchainFormat;
        VkSwapchainKHR           m_swapchain;
        std::vector<VkImage>     m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        uint                     m_currentIndex = 0;
        uint                     m_imageCount   = 0;
    };
} // namespace GE