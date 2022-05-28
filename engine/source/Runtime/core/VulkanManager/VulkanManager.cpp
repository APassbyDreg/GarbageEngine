#include "VulkanManager.h"


#include "function/Log/LogSystem.h"
#include "vk_Utils.h"

#define VKB_CHECK_RETURN(src, dest) \
    do \
    { \
        auto ret = src; \
        GE_CORE_ASSERT(ret.has_value(), "Failed to create Vulkan instance!"); \
        dest = ret.value(); \
    } while (false)

namespace GE
{
    VulkanManager::VulkanManager() {}

    VulkanManager::~VulkanManager() {}

    static VKAPI_ATTR VkBool32 VKAPI_CALL __debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void*                                       pUserData)
    {
        std::string msg_type;
        switch (messageTypes)
        {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                msg_type = "GENERAL";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                msg_type = "VALIDATION";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                msg_type = "PERFORMANCE";
                break;
            default:
                msg_type = "UNKNOWN";
                break;
        }

        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                GE_CORE_TRACE("[Vulkan {0}]: {1}", msg_type, pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                GE_CORE_INFO("[Vulkan {0}]: {1}", msg_type, pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                GE_CORE_WARN("[Vulkan {0}]: {1}", msg_type, pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                GE_CORE_ERROR("[Vulkan {0}]: {1}", msg_type, pCallbackData->pMessage);
                break;
            default:
                GE_CORE_WARN("[Vulkan {0}]: {1}", msg_type, pCallbackData->pMessage);
                break;
        }
        return VK_FALSE;
    }

    void VulkanManager::Init(GLFWwindow* window)
    {
        if (!m_initialized)
        {
            // init instance, surface, device, etc.
            init_vulkan(window);

            // init swapchain & images
            init_swapchain();
        }
        m_initialized = true;
    }

    void VulkanManager::Destroy()
    {
        if (m_initialized)
        {
            destroy_vulkan();

            destroy_swapchain();
        }
    }

    void VulkanManager::init_vulkan(GLFWwindow* window)
    {
        // create instance
        {
            if (!glfwVulkanSupported())
            {
                GE_CORE_CRITICAL("GLFW does not support Vulkan!");
            }

            // basic info
            vkb::InstanceBuilder builder;
            builder.set_app_name("GE")
                .set_app_version(1)
                .set_engine_name("GE")
                .set_engine_version(1)
                .require_api_version(VK_API_VERSION_1_3);

            // extensions
            uint32_t     glfw_extension_count = 0;
            const char** glfw_extensions      = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
            for (size_t i = 0; i < glfw_extension_count; i++)
            {
                builder.enable_extension(glfw_extensions[i]);
            }

#ifdef GE_DEBUG
            builder.request_validation_layers(true).set_debug_callback(__debug_messenger);
#endif
            VKB_CHECK_RETURN(builder.build(), m_vkbInstance);
            m_instance = m_vkbInstance.instance;

#ifdef GE_DEBUG
            m_debugMessenger = m_vkbInstance.debug_messenger;
#endif
        }

        // Create Surface
        {
            VK_CHECK_CRITICAL(glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface));
        }

        // Choose and create device
        {
            vkb::PhysicalDevice         vkb_physical_device;
            vkb::PhysicalDeviceSelector selector {m_vkbInstance};
            selector.set_minimum_version(1, 3).set_surface(m_surface).require_present().prefer_gpu_device_type(
                vkb::PreferredDeviceType::discrete);
            VKB_CHECK_RETURN(selector.select(), vkb_physical_device);

            vkb::Device        m_vkbDevice;
            vkb::DeviceBuilder builder {vkb_physical_device};
            VKB_CHECK_RETURN(builder.build(), m_vkbDevice);

            m_physicalDevice = vkb_physical_device.physical_device;
            m_device         = m_vkbDevice.device;
        }
    }

    void VulkanManager::destroy_vulkan()
    {
        vkb::destroy_device(m_vkbDevice);

#ifdef GE_DEBUG
        vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
#endif

        vkb::destroy_surface(m_vkbInstance, m_surface);

        vkb::destroy_instance(m_vkbInstance);
    }

    void VulkanManager::init_swapchain()
    {
        vkb::Swapchain        swapchain;
        vkb::SwapchainBuilder builder {m_physicalDevice, m_device, m_surface};
        builder.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
            .set_desired_format({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_extent(1920, 1080);
        VKB_CHECK_RETURN(builder.build(), swapchain);

        m_swapchain = swapchain.swapchain;
        VKB_CHECK_RETURN(swapchain.get_images(), m_swapchainImages);
        VKB_CHECK_RETURN(swapchain.get_image_views(), m_swapchainImageViews);
    }

    void VulkanManager::destroy_swapchain()
    {
        for (auto& imageView : m_swapchainImageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }

} // namespace GE
