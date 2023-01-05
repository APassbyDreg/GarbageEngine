#pragma once

#include "GE_pch.h"

#include "VulkanCommon.h"
#include "VulkanCreateInfoBuilder.h"

#include "GLFW/glfw3.h" // GLFW must be included after Vulkan
#include "VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct GE_API VkSupportStatus
    {
        bool hasComputeQueue               = false;
        bool hasTransferQueue              = false;
        bool hasDiscriteComputeQueue       = false;
        bool hasDiscriteTransferQueue      = false;
        bool isComputeAndTransferSameQueue = true;
    };

    class GE_API VulkanCore : public Singleton<VulkanCore>
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
        static inline VkPhysicalDevice GetPhysicalDevice() { return GetInstance().ensure_ready().m_physicalDevice; }
        static inline VkDevice         GetDevice() { return GetInstance().ensure_ready().m_device; }
        static inline VkSurfaceKHR     GetSurface() { return GetInstance().ensure_ready().m_surface; }

        static inline VkQueue GetGraphicsQueue() { return GetInstance().ensure_ready().m_graphicsQueue; }
        static inline VkQueue GetPresentQueue() { return GetInstance().ensure_ready().m_presentQueue; }
        static inline VkQueue GetComputeQueue() { return GetInstance().ensure_ready().m_computeQueue; }
        static inline VkQueue GetTransferQueue() { return GetInstance().ensure_ready().m_transferQueue; }

        static inline uint32_t GetGraphicsQueueFamilyIndex()
        {
            return GetInstance().ensure_ready().m_graphicsQueueFamilyIndex;
        }
        static inline uint32_t GetPresentQueueFamilyIndex()
        {
            return GetInstance().ensure_ready().m_presentQueueFamilyIndex;
        }
        static inline uint32_t GetComputeQueueFamilyIndex()
        {
            bool support = GetInstance().ensure_ready().m_supportStatus.hasComputeQueue;
            return support ? GetInstance().ensure_ready().m_computeQueueFamilyIndex : 0;
        }
        static inline uint32_t GetTransferQueueFamilyIndex()
        {
            bool support = GetInstance().ensure_ready().m_supportStatus.hasTransferQueue;
            return support ? GetInstance().ensure_ready().m_transferQueueFamilyIndex : 0;
        }

        /* ---------------------------- commands ---------------------------- */
        static inline VkCommandPool CreateGrahicsCmdPool()
        {
            VkCommandPoolCreateInfo create_info = VkInit::GetCommandPoolCreateInfo(GetGraphicsQueueFamilyIndex());
            VkCommandPool           command_pool;
            vkCreateCommandPool(GetDevice(), &create_info, nullptr, &command_pool);
            GetInstance().m_destroyActionStack.push_back(
                [=]() { vkDestroyCommandPool(GetDevice(), command_pool, nullptr); });
            return command_pool;
        }
        static inline VkCommandPool CreateComputeCmdPool()
        {
            VkCommandPoolCreateInfo create_info = VkInit::GetCommandPoolCreateInfo(GetComputeQueueFamilyIndex());
            VkCommandPool           command_pool;
            vkCreateCommandPool(GetDevice(), &create_info, nullptr, &command_pool);
            GetInstance().m_destroyActionStack.push_back(
                [=]() { vkDestroyCommandPool(GetDevice(), command_pool, nullptr); });
            return command_pool;
        }
        static inline std::vector<VkCommandBuffer>
        CreateCmdBuffer(VkCommandPool        pool,
                        size_t               count = 1,
                        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
        {
            VkCommandBufferAllocateInfo  info = VkInit::GetCommandBufferAllocateInfo(pool, level, 1);
            std::vector<VkCommandBuffer> cmd  = std::vector<VkCommandBuffer>(count);
            vkAllocateCommandBuffers(GetDevice(), &info, cmd.data());
            GetInstance().m_destroyActionStack.push_back(
                [=]() { vkFreeCommandBuffers(GetDevice(), pool, 1, cmd.data()); });
            return cmd;
        }
        static inline void ResetCmdPool(VkCommandPool pool, VkCommandPoolResetFlags flags = 0)
        {
            GE_VK_ASSERT(vkResetCommandPool(GetDevice(), pool, flags));
        }
        static inline void ResetCmdBuffer(VkCommandBuffer cmd, VkCommandBufferResetFlags flags = 0)
        {
            GE_VK_ASSERT(vkResetCommandBuffer(cmd, flags));
        }
        static inline VkCommandBuffer BeginTransferCmd()
        {
            VkCommandBuffer          cmd        = CreateCmdBuffer(GetInstance().m_transferCmdPool)[0];
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer(cmd, &begin_info);
            return cmd;
        }
        static inline void EndTransferCmd(VkCommandBuffer          cmd,
                                          std::vector<VkSemaphore> wait_semaphores   = {},
                                          std::vector<VkSemaphore> signal_semaphores = {},
                                          VkFence                  fence             = VK_NULL_HANDLE)
        {
            vkEndCommandBuffer(cmd);
            VkSubmitInfo submit_info         = {};
            submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount   = 1;
            submit_info.pCommandBuffers      = &cmd;
            submit_info.waitSemaphoreCount   = wait_semaphores.size();
            submit_info.pWaitSemaphores      = wait_semaphores.data();
            submit_info.signalSemaphoreCount = signal_semaphores.size();
            submit_info.pSignalSemaphores    = signal_semaphores.data();
            vkQueueSubmit(GetTransferQueue(), 1, &submit_info, fence);
            vkFreeCommandBuffers(GetDevice(), GetInstance().m_transferCmdPool, 1, &cmd);
        }

        /* ------------------------ synchronizations ------------------------ */
        static inline VkFence CreateFence()
        {
            VkFence           fence;
            VkFenceCreateInfo info = VkInit::GetFenceCreateInfo();
            GE_VK_ASSERT(vkCreateFence(GetDevice(), &info, nullptr, &fence));
            GetInstance().m_destroyActionStack.push_back([=]() { vkDestroyFence(GetDevice(), fence, nullptr); });
            return fence;
        }
        static inline VkSemaphore CreateSemaphore(VkSemaphoreCreateFlags flags = 0)
        {
            VkSemaphore           semaphore;
            VkSemaphoreCreateInfo info = VkInit::GetSemaphoreCreateInfo(flags);
            GE_VK_ASSERT(vkCreateSemaphore(GetDevice(), &info, nullptr, &semaphore));
            GetInstance().m_destroyActionStack.push_back(
                [=]() { vkDestroySemaphore(GetDevice(), semaphore, nullptr); });
            return semaphore;
        }
        static inline void WaitForFence(VkFence fence, bool waitAll = true, uint64_t timeout = UINT64_MAX)
        {
            VkBool32 waitFlag = waitAll ? VK_TRUE : VK_FALSE;
            GE_VK_CHECK(vkWaitForFences(GetDevice(), 1, &fence, waitFlag, timeout));
            GE_VK_CHECK(vkResetFences(GetDevice(), 1, &fence));
        }

        /* --------------------------- descriptors -------------------------- */
        static inline VkDescriptorPool GetGlobalDescriptorPool()
        {
            return GetInstance().ensure_ready().m_descriptorPool;
        }
        static inline VkDescriptorSetLayout CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo info)
        {
            VkDescriptorSetLayout layout;
            GE_VK_ASSERT(vkCreateDescriptorSetLayout(GetDevice(), &info, nullptr, &layout));
            GetInstance().m_destroyActionStack.push_back(
                [=]() { vkDestroyDescriptorSetLayout(GetDevice(), layout, nullptr); });
            return layout;
        }
        static inline std::vector<VkDescriptorSet> AllocDescriptorSets(VkDescriptorSetAllocateInfo info)
        {
            std::vector<VkDescriptorSet> sets(info.descriptorSetCount);
            info.descriptorPool = GetGlobalDescriptorPool();
            GE_VK_ASSERT(vkAllocateDescriptorSets(GetDevice(), &info, sets.data()));
            return sets;
        }

        /* ----------------------------- submit ----------------------------- */
        static inline void SubmitToGraphicsQueue(VkSubmitInfo info, VkFence fence = VK_NULL_HANDLE)
        {
            GE_VK_ASSERT(vkQueueSubmit(GetGraphicsQueue(), 1, &info, fence));
        }
        static inline void SubmitToComputeQueue(VkSubmitInfo info, VkFence fence = VK_NULL_HANDLE)
        {
            GE_VK_ASSERT(vkQueueSubmit(GetComputeQueue(), 1, &info, fence));
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

        std::vector<std::function<void()>> m_destroyActionStack;

        /* ---------------------- vulkan core resources --------------------- */
        VkSupportStatus          m_supportStatus;
        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice         m_physicalDevice;
        VkDevice                 m_device;
        VkQueue                  m_graphicsQueue, m_presentQueue;
        VkQueue                  m_computeQueue, m_transferQueue;
        uint32_t                 m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex;
        uint32_t                 m_computeQueueFamilyIndex, m_transferQueueFamilyIndex;
        VkSurfaceKHR             m_surface;
        VkDescriptorPool         m_descriptorPool;
        VkCommandPool            m_transferCmdPool;

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