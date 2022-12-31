#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

#include "vma/vk_mem_alloc.h"

namespace GE
{
    class GpuBuffer
    {
        const uint64_t c_waitTimeout = 100000000; // 0.1s

    public:
        GpuBuffer() { Setup(); }
        GpuBuffer(GpuBuffer& src);
        GpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);

        ~GpuBuffer();

        void Setup();

        void Upload(byte* data, size_t size, size_t offset = 0, bool resize = true);
        template<typename T>
        void UploadAs(std::vector<T>& data, size_t offset = 0, bool resize = true);

        void Download(byte* data, size_t size, size_t offset = 0);
        template<typename T>
        std::vector<T> DownloadAs(size_t count, size_t offset = 0);

        void Copy(GpuBuffer&               src_buffer,
                  size_t                   size             = 0,
                  size_t                   src_offset       = 0,
                  size_t                   dst_offset       = 0,
                  bool                     async            = false,
                  std::vector<VkSemaphore> wait_semaphores  = {},
                  bool                     resize_if_needed = true);

        void Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);
        void Destroy();

        void Resize(size_t size, size_t retain_start = 0, size_t retain_size = 0);

        inline size_t                  GetSize() { return m_bufferInfo.size; }
        inline VkBuffer                GetBuffer() { return m_buffer; }
        inline VkBufferCreateInfo      GetBufferInfo() { return m_bufferInfo; }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_allocInfo; }
        inline bool                    IsValid() { return m_alloced; }

        inline VkSemaphore GetSemaphore() { return m_hasAction ? m_actionCompleteSemaphore : VK_NULL_HANDLE; }

    private:
        inline void WaitLastAction()
        {
            if (m_hasAction)
            {
                VulkanCore::WaitForFence(m_actionCompleteFence, c_waitTimeout);
                m_hasAction = false;
            }
        }

    private:
        bool m_alloced = false;

        VkBufferCreateInfo      m_bufferInfo;
        VmaAllocationCreateInfo m_allocInfo;

        std::atomic_bool m_hasAction = false;
        VkFence          m_actionCompleteFence;
        VkSemaphore      m_actionCompleteSemaphore;

        VkBuffer      m_buffer;
        VmaAllocation m_allocation;
    };

} // namespace GE