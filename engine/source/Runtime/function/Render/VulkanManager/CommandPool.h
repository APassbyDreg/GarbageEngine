#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

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

        inline void Destroy()
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(VulkanCore::GetDevice(), pool, nullptr);
            }
        }

        inline operator VkCommandPool() { return pool; }

        inline CommandPool& operator=(VkCommandPoolCreateInfo info)
        {
            GE_CORE_ASSERT(pool == VK_NULL_HANDLE, "Re-assigning an created layout");
            Destroy();
            pool = VulkanCore::CreateCmdPool(info);
            return *this;
        }

        inline bool IsValid() { return pool != VK_NULL_HANDLE; }

    private:
        VkCommandPool pool = VK_NULL_HANDLE;
    };
} // namespace GE