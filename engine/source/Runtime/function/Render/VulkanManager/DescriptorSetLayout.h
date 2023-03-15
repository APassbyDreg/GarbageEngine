#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class DescriptorSetLayout
    {
    public:
        DescriptorSetLayout() {}
        DescriptorSetLayout(VkDescriptorSetLayoutCreateInfo info) { Create(info); }
        DescriptorSetLayout(DescriptorSetLayout&& old) { layout = old.layout; }

        ~DescriptorSetLayout() { Destroy(); }

        inline VkDescriptorSetLayout Get() { return layout; }

        inline void Create(VkDescriptorSetLayoutCreateInfo info)
        {
            layout = VulkanCore::CreateDescriptorSetLayout(info);
        }

        inline void Destroy()
        {
            if (layout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(VulkanCore::GetDevice(), layout, nullptr);
            }
        }

        inline operator VkDescriptorSetLayout() { return layout; }

        inline DescriptorSetLayout& operator=(VkDescriptorSetLayoutCreateInfo info)
        {
            GE_CORE_ASSERT(layout == VK_NULL_HANDLE, "Re-assigning an created layout");
            Destroy();
            layout = VulkanCore::CreateDescriptorSetLayout(info);
            return *this;
        }

        inline bool IsValid() { return layout != VK_NULL_HANDLE; }

    private:
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    };
} // namespace GE