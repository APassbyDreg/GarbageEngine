#pragma once

#include "GE_pch.h"
#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class Semaphore
    {
    public:
        Semaphore(uint flags = 0) { semaphore = VulkanCore::CreateSemaphore(flags); }
        ~Semaphore()
        {
            if (semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(VulkanCore::GetDevice(), semaphore, nullptr);
            }
        }

        inline VkSemaphore Get() { return semaphore; }
        inline             operator VkSemaphore() { return semaphore; }

    private:
        VkSemaphore semaphore = VK_NULL_HANDLE;
    };

    class Fence
    {
    public:
        Fence() { fence = VulkanCore::CreateFence(); }
        ~Fence()
        {
            if (fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(VulkanCore::GetDevice(), fence, nullptr);
            }
        }

        inline VkFence Get() { return fence; }
        inline         operator VkFence() { return fence; }

    private:
        VkFence fence = VK_NULL_HANDLE;
    };
} // namespace GE