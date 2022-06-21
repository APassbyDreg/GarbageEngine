#include "VulkanManager.h"

#include "Runtime/function/Log/LogSystem.h"

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
        }
        m_initialized = true;
    }

    void VulkanManager::Destroy()
    {
        if (m_initialized)
        {
            destroy_vulkan();
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
            VK_CHECK(glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface));
        }

        // Choose and create device
        {
            vkb::PhysicalDevice         m_vkbPhysicalDevice;
            vkb::PhysicalDeviceSelector selector {m_vkbInstance};
            selector.set_minimum_version(1, 3).set_surface(m_surface).require_present().prefer_gpu_device_type(
                vkb::PreferredDeviceType::discrete);
            VKB_CHECK_RETURN(selector.select(), m_vkbPhysicalDevice);

            // REVIEW: is multiple queue needed?

            vkb::Device        m_vkbDevice;
            vkb::DeviceBuilder builder {m_vkbPhysicalDevice};
            VKB_CHECK_RETURN(builder.build(), m_vkbDevice);

            m_physicalDevice = m_vkbPhysicalDevice.physical_device;
            m_device         = m_vkbDevice.device;

            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::graphics), m_graphicsQueue);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::present), m_presentQueue);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::compute), m_computeQueue);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::transfer), m_transferQueue);

            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::graphics), m_graphicsQueueFamilyIndex);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::present), m_presentQueueFamilyIndex);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::compute), m_computeQueueFamilyIndex);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::transfer), m_transferQueueFamilyIndex);
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

    void VulkanManager::init_vma()
    {
        VmaVulkanFunctions vulkanFunctions    = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion       = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice         = m_physicalDevice;
        allocatorCreateInfo.device                 = m_device;
        allocatorCreateInfo.instance               = m_instance;
        allocatorCreateInfo.pVulkanFunctions       = &vulkanFunctions;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);
    }

    void VulkanManager::destroy_vma() { vmaDestroyAllocator(m_allocator); }

    // void VulkanManager::init_swapchain(int2 size)
    // {
    //     vkb::Swapchain        swapchain;
    //     vkb::SwapchainBuilder builder {m_physicalDevice, m_device, m_surface};
    //     builder.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
    //         .set_desired_format({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
    //         .set_desired_extent(size.x, size.y);
    //     VKB_CHECK_RETURN(builder.build(), swapchain);

    //     m_swapchain       = swapchain.swapchain;
    //     m_swapchainExtent = swapchain.extent;
    //     m_swapchainFormat = swapchain.image_format;
    //     VKB_CHECK_RETURN(swapchain.get_images(), m_swapchainImages);
    //     VKB_CHECK_RETURN(swapchain.get_image_views(), m_swapchainImageViews);
    // }

    // void VulkanManager::destroy_swapchain()
    // {
    //     for (auto& imageView : m_swapchainImageViews)
    //     {
    //         vkDestroyImageView(m_device, imageView, nullptr);
    //     }

    //     vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    // }

} // namespace GE
