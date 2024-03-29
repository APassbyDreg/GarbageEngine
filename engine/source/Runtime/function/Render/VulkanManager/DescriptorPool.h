#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"

namespace GE
{
    class DescriptorPool
    {
    public:
        DescriptorPool() {}
        DescriptorPool(VkDescriptorPoolCreateInfo info)
        {
            pool = VulkanCore::CreateDescriptorPool(info);
        }
        DescriptorPool(DescriptorPool&& old) { pool = old.pool; }

        ~DescriptorPool() { Destroy(); }

        inline VkDescriptorPool Get() { return pool; }

        inline void Destroy()
        {
            GE_CORE_ASSERT(
                VulkanCore::IsAlive(), "DescriptorPool {} should be destroyed before VulkanCore", (void*)pool);
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(VulkanCore::GetDevice(), pool, nullptr);
            }
        }

        inline operator VkDescriptorPool() { return pool; }

        inline DescriptorPool& operator=(VkDescriptorPoolCreateInfo info)
        {
            GE_CORE_ASSERT(pool == VK_NULL_HANDLE, "Re-assigning an created pool");
            Destroy();
            pool = VulkanCore::CreateDescriptorPool(info);
            return *this;
        }

        inline bool IsValid() { return pool != VK_NULL_HANDLE; }

    private:
        VkDescriptorPool pool = VK_NULL_HANDLE;
    };
} // namespace GE