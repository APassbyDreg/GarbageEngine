#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class FrameBuffer
    {
    public:
        FrameBuffer() {}
        FrameBuffer(VkFramebufferCreateInfo info) { framebuffer = VulkanCore::CreateFramebuffer(info); }
        ~FrameBuffer()
        {
            if (framebuffer != VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer(VulkanCore::GetDevice(), framebuffer, nullptr);
            }
        }

        inline VkFramebuffer Get() { return framebuffer; }
        inline               operator VkFramebuffer() { return framebuffer; }

        inline bool IsValid() { return framebuffer != VK_NULL_HANDLE; }

    private:
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
    };
} // namespace GE