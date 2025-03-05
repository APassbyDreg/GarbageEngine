#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"

namespace GE
{
    class Sampler
    {
    public:
        Sampler() {}
        Sampler(VkSamplerCreateInfo info) { sampler = VulkanCore::CreateSampler(info); }
        Sampler(Sampler&& old) { sampler = old.sampler; }

        ~Sampler() { Destroy(); }

        inline VkSampler Get() { return sampler; }

        inline void Destroy()
        {
            if (sampler != VK_NULL_HANDLE)
            {
                GE_CORE_ASSERT(VulkanCore::IsAlive(), "Sampler {} should be destroyed before VulkanCore", (void*)sampler);
                vkDestroySampler(VulkanCore::GetDevice(), sampler, nullptr);
                sampler = VK_NULL_HANDLE;
            }
        }

        inline operator VkSampler() { return sampler; }

        inline Sampler& operator=(VkSamplerCreateInfo info)
        {
            GE_CORE_ASSERT(sampler == VK_NULL_HANDLE, "Re-assigning an created sampler");
            Destroy();
            sampler = VulkanCore::CreateSampler(info);
            return *this;
        }

        inline bool IsValid() { return sampler != VK_NULL_HANDLE; }

    private:
        VkSampler sampler = VK_NULL_HANDLE;
    };
} // namespace GE