#include "VulkanSwapchain.h"

namespace GE
{
    VulkanSwapchain::VulkanSwapchain(VkExtent2D extent) : m_swapchainExtent(extent) { __create_swapchain(); }

    VulkanSwapchain::~VulkanSwapchain() { __destroy_swapchain(); }

    void VulkanSwapchain::Resize(VkExtent2D extent)
    {
        m_swapchainExtent = extent;
        __destroy_swapchain();
        __create_swapchain();
    }

    void VulkanSwapchain::__create_swapchain()
    {
        vkb::Swapchain        swapchain;
        vkb::SwapchainBuilder builder {
            VulkanCore::GetPhysicalDevice(), VulkanCore::GetDevice(), VulkanCore::GetSurface()};
        builder.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
            .set_desired_format({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_extent(m_swapchainExtent.width, m_swapchainExtent.height);
        VKB_CHECK_RETURN(builder.build(), swapchain);

        m_swapchain       = swapchain.swapchain;
        m_swapchainFormat = swapchain.image_format;
        VKB_CHECK_RETURN(swapchain.get_images(), m_swapchainImages);
        VKB_CHECK_RETURN(swapchain.get_image_views(), m_swapchainImageViews);

        m_imageCount = m_swapchainImages.size();

        // Create semaphores to wait for image available and render finished
        for (size_t i = 0; i < m_swapchainImages.size(); i++)
        {
            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkSemaphore image_available_semaphore;
            GE_VK_ASSERT(
                vkCreateSemaphore(VulkanCore::GetDevice(), &semaphore_info, nullptr, &image_available_semaphore));
            m_imageAvailableSemaphores.push_back(image_available_semaphore);
        }

        m_ready = true;
    }

    void VulkanSwapchain::__destroy_swapchain()
    {
        for (auto& imageView : m_swapchainImageViews)
        {
            vkDestroyImageView(VulkanCore::GetDevice(), imageView, nullptr);
        }
        vkDestroySwapchainKHR(VulkanCore::GetDevice(), m_swapchain, nullptr);

        m_ready = false;
    }
} // namespace GE
