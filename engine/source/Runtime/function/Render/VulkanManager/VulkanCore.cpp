#include "VulkanCore.h"

#include "Runtime/function/Log/LogSystem.h"

namespace GE
{
    VulkanCore::VulkanCore() {}

    VulkanCore::~VulkanCore() { Destroy(); }

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

    void VulkanCore::Init(GLFWwindow* window)
    {
        if (!m_ready)
        {
            // init instance, surface, device, etc.
            init_vulkan(window);
            // init vma
            init_vma();
        }
        m_ready = true;
    }

    void VulkanCore::Destroy()
    {
        if (m_ready)
        {
            destroy_vma();
            destroy_vulkan();
        }
    }

    void VulkanCore::init_vulkan(GLFWwindow* window)
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
                .require_api_version(VK_API_VERSION_1_2);

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
            m_destroyActionStack.push_back([this]() { vkb::destroy_instance(m_vkbInstance); });
#ifdef GE_DEBUG
            m_debugMessenger = m_vkbInstance.debug_messenger;
            m_destroyActionStack.push_back(
                [this]() { vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger); });
#endif
        }

        // Create Surface
        {
            GE_VK_ASSERT(glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface));

            // add destroy action
            m_destroyActionStack.push_back([this]() { vkb::destroy_surface(m_vkbInstance, m_surface); });
        }

        // Choose and create device
        {
            vkb::PhysicalDeviceSelector selector {m_vkbInstance};
            selector.set_minimum_version(1, 2).set_surface(m_surface).require_present().prefer_gpu_device_type(
                vkb::PreferredDeviceType::discrete);
            VKB_CHECK_RETURN(selector.select(), m_vkbPhysicalDevice);

            // REVIEW: is multiple queue needed?

            vkb::DeviceBuilder builder {m_vkbPhysicalDevice};
            VKB_CHECK_RETURN(builder.build(), m_vkbDevice);

            m_physicalDevice = m_vkbPhysicalDevice.physical_device;
            m_device         = m_vkbDevice.device;

            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::graphics), m_graphicsQueue);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::graphics), m_graphicsQueueFamilyIndex);

            VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::present), m_presentQueue);
            VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::present), m_presentQueueFamilyIndex);

            try
            {
                VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::compute, false), m_computeQueue);
                VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::compute, false),
                                 m_computeQueueFamilyIndex);
                m_supportStatus.hasComputeQueue = true;
            }
            catch (const std::exception& e)
            {
                GE_CORE_ERROR("Failed to get compute queue: {}", e.what());
            }

            try
            {
                VKB_CHECK_RETURN(m_vkbDevice.get_queue(vkb::QueueType::transfer), m_transferQueue);
                VKB_CHECK_RETURN(m_vkbDevice.get_queue_index(vkb::QueueType::transfer), m_transferQueueFamilyIndex);
                m_supportStatus.hasTransferQueue = true;
            }
            catch (const std::exception& e)
            {
                GE_CORE_ERROR("Failed to get transfer queue: {}", e.what());
            }

            // write support status
            m_supportStatus.isComputeAndTransferSameQueue = m_computeQueue == m_transferQueue;
            m_supportStatus.hasDiscriteComputeQueue =
                m_computeQueue != m_graphicsQueue && m_computeQueue != m_transferQueue;
            m_supportStatus.hasDiscriteTransferQueue =
                m_transferQueue != m_graphicsQueue && m_transferQueue != m_computeQueue;

            // add destroy action
            m_destroyActionStack.push_back([&]() { vkb::destroy_device(m_vkbDevice); });
        }

        // create global descriptor pool
        {
            VkDescriptorPoolSize       pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                       {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                       {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                       {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                       {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
            VkDescriptorPoolCreateInfo pool_info    = {};
            pool_info.sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags                         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets                       = 1000 * 11;
            pool_info.poolSizeCount                 = 11;
            pool_info.pPoolSizes                    = pool_sizes;
            GE_VK_ASSERT(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptorPool));
            m_destroyActionStack.push_back([this]() { vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr); });
        }

        // create global transfer command pool
        if (m_supportStatus.hasTransferQueue)
        {
            auto info = VkInit::GetCommandPoolCreateInfo(m_transferQueueFamilyIndex);
            GE_VK_ASSERT(vkCreateCommandPool(m_device, &info, nullptr, &m_transferCmdPool));
            m_destroyActionStack.push_back([this]() { vkDestroyCommandPool(m_device, m_transferCmdPool, nullptr); });
        }
    }

    void VulkanCore::destroy_vulkan()
    {
        GE_VK_ASSERT(vkDeviceWaitIdle(m_device));

        // run destroy stack back to front
        for (int i = m_destroyActionStack.size() - 1; i >= 0; i--)
        {
            m_destroyActionStack[i]();
        }
    }

    void VulkanCore::init_vma()
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
        GE_VK_ASSERT(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator));
    }

    void VulkanCore::destroy_vma() { vmaDestroyAllocator(m_allocator); }

} // namespace GE
