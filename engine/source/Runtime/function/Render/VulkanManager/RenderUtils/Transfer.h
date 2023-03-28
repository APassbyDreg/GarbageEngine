#pragma once

#include "GE_pch.h"

#include "../CommandPool.h"
#include "../Synchronization.h"
#include "../VulkanCore.h"

#include "Runtime/core/Time.h"
#include <atomic>

namespace GE
{
    namespace RenderUtils
    {
        class OneTimeTransferContext final
        {
        public:
            OneTimeTransferContext()                             = delete;
            OneTimeTransferContext(OneTimeTransferContext& ctx)  = delete;
            OneTimeTransferContext(OneTimeTransferContext&& ctx) = delete;

            inline OneTimeTransferContext(VkCommandBuffer   _cmd,
                                          VkCommandPool     _pool,
                                          VkFence           _fence,
                                          std::atomic_bool& _occupied) :
                cmd(_cmd),
                pool(_pool), fence(_fence), occupied(_occupied)
            {}

            inline void Run(std::function<void(VkCommandBuffer)> fn)
            {
                while (occupied)
                {
                    Time::Sleep(0.001);
                }
                occupied = true;
                if (!valid)
                {
                    GE_CORE_CRITICAL("[OneTimeTransferContext::Run]: trying to run an invalid ctx");
                    return;
                }

                // begin cmd
                {
                    VkCommandBufferBeginInfo info = {};
                    info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                    GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &info));
                }

                fn(cmd);

                // end and submit cmd
                {
                    GE_VK_ASSERT(vkEndCommandBuffer(cmd));
                    VkSubmitInfo info         = {};
                    info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    info.pNext                = nullptr;
                    info.waitSemaphoreCount   = 0;
                    info.pWaitSemaphores      = nullptr;
                    info.pWaitDstStageMask    = nullptr;
                    info.commandBufferCount   = 1;
                    info.pCommandBuffers      = &cmd;
                    info.signalSemaphoreCount = 0;
                    info.pSignalSemaphores    = nullptr;
                    VulkanCore::SubmitToTransferQueue(info, fence);
                }

                // clean up
                VulkanCore::WaitForFence(fence);
                GE_VK_ASSERT(
                    vkResetCommandPool(VulkanCore::GetDevice(), pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
                valid    = false;
                occupied = false;
            }

        private:
            bool              valid = true;
            VkCommandBuffer   cmd;
            VkCommandPool     pool;
            VkFence           fence;
            std::atomic_bool& occupied;
        };

        static OneTimeTransferContext GetOneTimeTransferContext()
        {
            static std::atomic_bool occupied = false;
            static CommandPool     pool = {VkInit::GetCommandPoolCreateInfo(VulkanCore::GetTransferQueueFamilyIndex())};
            static Fence           fence = {};
            static VkCommandBuffer cmd   = VK_NULL_HANDLE;
            if (cmd == VK_NULL_HANDLE)
            {
                cmd = VulkanCore::CreateCmdBuffers(pool, 1)[0];
            }
            return {cmd, pool, fence, occupied};
        }

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
            // begin cmd
            {
                VkCommandBufferBeginInfo info = {};
                info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                GE_VK_ASSERT(vkBeginCommandBuffer(cmd, &info));
            }
            // do transfer
            {
                VkBufferCopy copyRegion {};
                copyRegion.srcOffset = src_offset;
                copyRegion.dstOffset = dst_offset;
                copyRegion.size      = size;
                vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);
            }
            // end cmd and submit
            {
                GE_VK_ASSERT(vkEndCommandBuffer(cmd));
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