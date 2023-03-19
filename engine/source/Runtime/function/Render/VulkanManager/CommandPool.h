#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"

namespace GE
{

    class CommandPool
    {
    public:
        CommandPool() {}
        CommandPool(VkCommandPoolCreateInfo info) { pool = VulkanCore::CreateCmdPool(info); }
        CommandPool(CommandPool&& old) { pool = old.pool; }

        ~CommandPool() { Destroy(); }

        inline VkCommandPool Get() { return pool; }

        inline CommandPool& Create(VkCommandPoolCreateInfo info)
        {
            GE_CORE_ASSERT(pool == VK_NULL_HANDLE, "Cannot re-create a created pool");
            pool = VulkanCore::CreateCmdPool(info);
            return *this;
        }

        inline void Destroy()
        {
            GE_CORE_ASSERT(VulkanCore::IsAlive(), "CommandPool {} should be destroyed before VulkanCore", (void*)pool);
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(VulkanCore::GetDevice(), pool, nullptr);
            }
        }

        inline operator VkCommandPool() { return pool; }

        inline CommandPool& operator=(VkCommandPoolCreateInfo info) { return Create(info); }

        inline bool IsValid() { return pool != VK_NULL_HANDLE; }

    private:
        VkCommandPool pool = VK_NULL_HANDLE;
    };
} // namespace GE