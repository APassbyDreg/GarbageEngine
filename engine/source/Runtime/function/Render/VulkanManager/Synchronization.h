#pragma once

#include "GE_pch.h"
#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class Semaphore
    {
    public:
        Semaphore(Semaphore&& old) { semaphore = old.semaphore; }
        Semaphore(uint flags = 0) { semaphore = VulkanCore::CreateSemaphore(flags); }
        ~Semaphore()
        {
            if (semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(VulkanCore::GetDevice(), semaphore, nullptr);
            }
        }

        inline VkSemaphore Get() { return semaphore; }
        inline bool        IsValid() { return semaphore != VK_NULL_HANDLE; }
        inline             operator VkSemaphore() { return semaphore; }

    private:
        VkSemaphore semaphore = VK_NULL_HANDLE;
    };

    class Fence
    {
    public:
        Fence() { fence = VulkanCore::CreateFence(); }
        Fence(Fence&& old) { fence = old.fence; }
        ~Fence()
        {
            if (fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(VulkanCore::GetDevice(), fence, nullptr);
            }
        }

        inline VkFence Get() { return fence; }
        inline bool    IsValid() { return fence != VK_NULL_HANDLE; }
        inline         operator VkFence() { return fence; }

    private:
        VkFence fence = VK_NULL_HANDLE;
    };
} // namespace GE