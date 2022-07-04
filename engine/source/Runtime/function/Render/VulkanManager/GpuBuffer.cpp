#include "GpuBuffer.h"

namespace GE
{
    GpuBuffer::GpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info) :
        m_bufferInfo(buffer_info), m_allocInfo(alloc_info)
    {
        VK_CHECK(
            vmaCreateBuffer(VulkanCore::GetAllocator(), &buffer_info, &alloc_info, &m_buffer, &m_allocation, nullptr));

        m_alloced = true;
    }

    GpuBuffer::GpuBuffer(GpuBuffer& src) : m_bufferInfo(src.m_bufferInfo), m_allocInfo(src.m_allocInfo)
    {
        VK_CHECK(vmaCreateBuffer(
            VulkanCore::GetAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));

        m_alloced = true;
    }

    GpuBuffer::~GpuBuffer()
    {
        vmaDestroyBuffer(VulkanCore::GetAllocator(), m_buffer, m_allocation);
        m_alloced = false;
    }

    void GpuBuffer::Upload(void* data, size_t& size)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

        void* mapped;
        VK_CHECK(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, &mapped));
        memcpy(mapped, data, size);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }

    void GpuBuffer::Download(void* data, size_t& size)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

        size = m_bufferInfo.size;

        void* mapped;
        VK_CHECK(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, &mapped));
        memcpy(data, mapped, size);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }

    void GpuBuffer::Clone(GpuBuffer& buffer)
    {
        // TODO: use a gpu compute kernel to do this

        if (m_alloced && GetSize() != buffer.GetSize())
        {
            vmaDestroyBuffer(VulkanCore::GetAllocator(), m_buffer, m_allocation);
            m_alloced = false;
        }

        if (!m_alloced)
        {
            m_bufferInfo = buffer.GetBufferInfo();
            m_allocInfo  = buffer.GetAllocInfo();
            VK_CHECK(vmaCreateBuffer(
                VulkanCore::GetAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));
            m_alloced = true;
        }

        void *src, *dst;
        vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, &dst);
        vmaMapMemory(VulkanCore::GetAllocator(), buffer.m_allocation, &src);
        memcpy(dst, src, GetSize());
        vmaUnmapMemory(VulkanCore::GetAllocator(), buffer.m_allocation);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }
} // namespace GE