#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

#include "vma/vk_mem_alloc.h"

namespace GE
{
    class GpuBuffer
    {
    public:
        GpuBuffer() {}
        GpuBuffer(GpuBuffer& src);
        GpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);

        ~GpuBuffer();

        void Upload(void* data, size_t& size);

        void Download(void* data, size_t& size);

        void Clone(GpuBuffer& buffer);

        inline size_t   GetSize() { return m_bufferInfo.size; }
        inline VkBuffer GetBuffer() { return m_buffer; }

        inline VkBufferCreateInfo      GetBufferInfo() { return m_bufferInfo; }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_allocInfo; }

    private:
        bool m_alloced = false;

        VkBufferCreateInfo      m_bufferInfo;
        VmaAllocationCreateInfo m_allocInfo;

        VkBuffer      m_buffer;
        VmaAllocation m_allocation;
    };

} // namespace GE