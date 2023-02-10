#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    namespace RenderUtils
    {
        static VkImageSubresourceRange AllImageSubresourceRange(VkImageAspectFlags aspectFlag)
        {
            VkImageSubresourceRange ranges = {};
            ranges.aspectMask              = aspectFlag;
            ranges.baseMipLevel            = 0;
            ranges.levelCount              = VK_REMAINING_MIP_LEVELS;
            ranges.baseArrayLayer          = 0;
            ranges.layerCount              = VK_REMAINING_ARRAY_LAYERS;
            return ranges;
        }

        static void TransitionImageLayout(VkCommandBuffer         cmd,
                                          VkImage                 image,
                                          VkImageLayout           oldLayout,
                                          VkImageLayout           newLayout,
                                          VkImageSubresourceRange range)
        {
            VkImageMemoryBarrier barrier {};
            barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image               = image;
            barrier.oldLayout           = oldLayout;
            barrier.newLayout           = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange    = range;
            barrier.srcAccessMask       = 0; // TODO
            barrier.dstAccessMask       = 0; // TODO
            vkCmdPipelineBarrier(cmd,
                                 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
        }

        static void BeginOneTimeSubmitCmdBuffer(VkCommandBuffer cmd)
        {
            VkCommandBufferBeginInfo cmd_info = {};
            cmd_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &cmd_info));
        }

        static void EndCmdBuffer(VkCommandBuffer cmd) { GE_VK_ASSERT(vkEndCommandBuffer(cmd)); }

        static void TransferBuffer(VkCommandBuffer          cmd, // should come from a transfer queue
                                   VkBuffer                 src,
                                   VkBuffer                 dst,
                                   size_t                   size,
                                   size_t                   src_offset        = 0,
                                   size_t                   dst_offset        = 0,
                                   std::vector<VkSemaphore> wait_semaphores   = {},
                                   std::vector<VkSemaphore> signal_semaphores = {},
                                   VkFence                  fence             = VK_NULL_HANDLE,
                                   VkPipelineStageFlags     wait_stages       = VK_PIPELINE_STAGE_NONE)
        {
            // record
            {
                BeginOneTimeSubmitCmdBuffer(cmd);
                VkBufferCopy copyRegion {};
                copyRegion.srcOffset = src_offset;
                copyRegion.dstOffset = dst_offset;
                copyRegion.size      = size;
                vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);
                EndCmdBuffer(cmd);
            }
            // submit
            {
                VkSubmitInfo info         = {};
                info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.commandBufferCount   = 1;
                info.pCommandBuffers      = &cmd;
                info.signalSemaphoreCount = signal_semaphores.size();
                info.pSignalSemaphores    = signal_semaphores.data();
                info.waitSemaphoreCount   = wait_semaphores.size();
                info.pWaitSemaphores      = wait_semaphores.data();
                VulkanCore::SubmitToTransferQueue(info, fence);
            }
        }
    } // namespace RenderUtils
} // namespace GE