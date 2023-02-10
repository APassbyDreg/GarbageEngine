#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class GE_API GpuBuffer
    {
        const uint64_t c_waitTimeout = 100000000; // 0.1s

    public:
        GpuBuffer() { Setup(); }
        GpuBuffer(GpuBuffer& src);
        GpuBuffer(GpuBuffer&& src);
        GpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);

        ~GpuBuffer();

        void Setup();

        void Upload(byte* data, size_t size, size_t offset = 0, bool resize = true);
        void Download(byte* data, size_t size, size_t offset = 0);

        template<typename T>
        void UploadAs(std::vector<T>& data, size_t offset = 0, bool resize = true)
        {
            GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

            const size_t size_in_bytes   = data.size() * sizeof(T);
            const size_t offset_in_bytes = offset * sizeof(T);

            if (resize && offset_in_bytes + size_in_bytes > GetSize())
            {
                Resize(offset_in_bytes + size_in_bytes, 0, offset_in_bytes);
            }

            T* mapped;
            GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
            memcpy(mapped + offset, data.data(), size_in_bytes);
            vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
        }
        template<typename T>
        std::vector<T> DownloadAs(size_t count, size_t offset = 0)
        {
            GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

            std::vector<T> result;
            result.resize(count);

            T* mapped;
            GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
            memcpy(result.data(), mapped + offset, count * sizeof(T));
            vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);

            return result;
        }

        void Copy(GpuBuffer& src_buffer,
                  size_t     size             = 0,
                  size_t     src_offset       = 0,
                  size_t     dst_offset       = 0,
                  bool       resize_if_needed = true);
        void Resize(size_t size, size_t retain_start = 0, size_t retain_size = 0);

        void Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);
        void Destroy();

        inline size_t                  GetSize() { return m_bufferInfo.size; }
        inline VkBuffer                GetBuffer() { return m_buffer; }
        inline VkBufferCreateInfo      GetBufferInfo() { return m_bufferInfo; }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_allocInfo; }
        inline bool                    IsValid() { return m_alloced; }

    private:
        static VkCommandPool GetBufferTransferCmdPool()
        {
            static VkCommandPool pool = VK_NULL_HANDLE;
            if (pool == VK_NULL_HANDLE)
            {
                pool = VulkanCore::CreateTransferCmdPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                                         VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
            }
            return pool;
        }

    private:
        bool m_alloced = false;

        VkBufferCreateInfo      m_bufferInfo;
        VmaAllocationCreateInfo m_allocInfo;

        VkFence         m_actionCompleteFence = VK_NULL_HANDLE;
        VkCommandBuffer m_transferCmd         = VK_NULL_HANDLE;

        VkBuffer      m_buffer, m_oldBuffer;
        VmaAllocation m_allocation;
    };

} // namespace GE