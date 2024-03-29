#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

#include "RenderUtils/Transfer.h"
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
                                          VkImageSubresourceRange range,
                                          VkAccessFlags           srcAccessMask = 0,
                                          VkAccessFlags           dstAccessMask = 0,
                                          VkPipelineStageFlags    srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                          VkPipelineStageFlags    dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
        {
            VkImageMemoryBarrier barrier {};
            barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image               = image;
            barrier.oldLayout           = oldLayout;
            barrier.newLayout           = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange    = range;
            barrier.srcAccessMask       = srcAccessMask;
            barrier.dstAccessMask       = dstAccessMask;
            vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        static void BeginOneTimeSubmitCmdBuffer(VkCommandBuffer cmd)
        {
            VkCommandBufferBeginInfo cmd_info = {};
            cmd_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &cmd_info));
        }

        static void EndCmdBuffer(VkCommandBuffer cmd) { GE_VK_ASSERT(vkEndCommandBuffer(cmd)); }

    } // namespace RenderUtils
} // namespace GE