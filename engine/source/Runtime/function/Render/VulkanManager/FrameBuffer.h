#pragma once

#include "GE_pch.h"
#include "GpuImage.h"
#include "VulkanCore.h"

namespace GE
{
    class FrameBuffer
    {
    public:
        FrameBuffer() {}
        FrameBuffer(FrameBuffer&& old) { framebuffer = old.framebuffer; }
        FrameBuffer(VkFramebufferCreateInfo info) { framebuffer = VulkanCore::CreateFramebuffer(info); }
        ~FrameBuffer()
        {
            GE_CORE_ASSERT(
                VulkanCore::IsAlive(), "Framebuffer {} should be destroyed before VulkanCore", (void*)framebuffer);
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