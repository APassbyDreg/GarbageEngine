#include "GpuBuffer.h"

namespace GE
{
    void TransferBuffer(VkBuffer                 src,
                        VkBuffer                 dst,
                        size_t                   size,
                        size_t                   src_offset        = 0,
                        size_t                   dst_offset        = 0,
                        std::vector<VkSemaphore> wait_semaphores   = {},
                        std::vector<VkSemaphore> signal_semaphores = {},
                        VkFence                  fence             = VK_NULL_HANDLE)
    {
        VkCommandBuffer cmd = VulkanCore::BeginTransferCmd();
        VkBufferCopy    copyRegion {};
        copyRegion.srcOffset = src_offset;
        copyRegion.dstOffset = dst_offset;
        copyRegion.size      = size;
        vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);
        VulkanCore::EndTransferCmd(cmd, wait_semaphores, signal_semaphores, fence);
    }

    void GpuBuffer::Setup()
    {
        m_actionCompleteSemaphore = VulkanCore::CreateSemaphore();
        m_actionCompleteFence     = VulkanCore::CreateFence();
    }

    GpuBuffer::GpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info)
    {
        Alloc(buffer_info, alloc_info);
        Setup();
    }

    GpuBuffer::GpuBuffer(GpuBuffer& src)
    {
        Alloc(src.m_bufferInfo, src.m_allocInfo);
        Copy(src);
        Setup();
    }

    GpuBuffer::~GpuBuffer() { Destroy(); }

    void GpuBuffer::Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info)
    {
        Destroy();

        m_bufferInfo = buffer_info;
        m_allocInfo  = alloc_info;
        GE_VK_ASSERT(
            vmaCreateBuffer(VulkanCore::GetAllocator(), &buffer_info, &alloc_info, &m_buffer, &m_allocation, nullptr));
        m_alloced = true;
    }

    void GpuBuffer::Destroy()
    {
        if (m_alloced)
        {
            vmaDestroyBuffer(VulkanCore::GetAllocator(), m_buffer, m_allocation);
            m_alloced = false;
        }
    }

    void GpuBuffer::Upload(byte* data, size_t size, size_t offset, bool resize)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

        if (resize)
        {
            Resize(size + offset, 0, offset);
        }

        GE_CORE_ASSERT(
            size + offset <= GetSize(), "Upload size is too large! ({} + {} -> {})", size, offset, GetSize());

        byte* mapped;
        GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
        memcpy(mapped, data + offset, size);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }

    template<typename T>
    void GpuBuffer::UploadAs(std::vector<T>& data, size_t offset, bool resize)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");

        const size_t size_in_bytes   = data.size() * sizeof(T);
        const size_t offset_in_bytes = offset * sizeof(T);

        if (resize)
        {
            Resize(offset_in_bytes + size_in_bytes, 0, offset_in_bytes);
        }

        T* mapped;
        GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
        memcpy(mapped + offset, data.data(), size_in_bytes);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }

    void GpuBuffer::Download(byte* data, size_t size, size_t offset)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");
        WaitLastAction();

        byte* mapped;
        GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
        memcpy(data, mapped + offset, size);
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);
    }

    template<typename T>
    std::vector<T> GpuBuffer::DownloadAs(size_t count, size_t offset)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");
        WaitLastAction();

        std::vector<T> result;
        result.resize(count);

        T* mapped;
        GE_VK_ASSERT(vmaMapMemory(VulkanCore::GetAllocator(), m_allocation, (void**)&mapped));
        memcpy(result.data(), mapped + offset, count * sizeof(T));
        vmaUnmapMemory(VulkanCore::GetAllocator(), m_allocation);

        return result;
    }

    void GpuBuffer::Copy(GpuBuffer&               src_buffer,
                         size_t                   size,
                         size_t                   src_offset,
                         size_t                   dst_offset,
                         bool                     async,
                         std::vector<VkSemaphore> wait_semaphores,
                         bool                     resize_if_needed)
    {
        GE_CORE_ASSERT(src_buffer.IsValid(), "Src buffer is not valid");

        if (size == 0)
        {
            size = src_buffer.GetSize();
        }

        GE_CORE_ASSERT(size + src_offset <= src_buffer.GetSize(),
                       "Not enough src buffer size to clone: size={}, src_offset={}, buffer_size={}",
                       size,
                       src_offset,
                       src_buffer.GetSize());
        if (size + dst_offset > GetSize())
        {
            GE_CORE_ASSERT(resize_if_needed,
                           "Not enough dst buffer size to clone: size={}, dst_offset={}, buffer_size={}",
                           size,
                           dst_offset,
                           GetSize());
            Resize(size + dst_offset, 0, dst_offset);
        }

        if (!m_alloced)
        {
            m_bufferInfo = src_buffer.GetBufferInfo();
            m_allocInfo  = src_buffer.GetAllocInfo();
            GE_VK_ASSERT(vmaCreateBuffer(
                VulkanCore::GetAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));
            m_alloced = true;
        }

        // dispatch transfer
        WaitLastAction();
        TransferBuffer(src_buffer.m_buffer,
                       m_buffer,
                       size,
                       src_offset,
                       dst_offset,
                       wait_semaphores,
                       {m_actionCompleteSemaphore},
                       m_actionCompleteFence);
        m_hasAction = true;
        if (!async)
        {
            WaitLastAction();
        }
    }

    void GpuBuffer::Resize(size_t size, size_t retain_start, size_t retain_size)
    {
        GE_CORE_ASSERT(m_alloced, "GpuBuffer is not alloced!");
        GE_CORE_ASSERT(retain_start <= m_bufferInfo.size,
                       "Retain start larger than original size! ({} -> {})",
                       retain_start,
                       m_bufferInfo.size);
        GE_CORE_ASSERT(retain_start + retain_size <= m_bufferInfo.size,
                       "Retain end is larger than original size! ({} -> {})",
                       retain_start + retain_size,
                       m_bufferInfo.size);
        GE_CORE_ASSERT(retain_start + retain_size <= size,
                       "Retain end is larger than new size! ({} -> {})",
                       retain_start + retain_size,
                       size);

        m_bufferInfo.size = size;
        if (retain_size == 0)
        {
            vmaDestroyBuffer(VulkanCore::GetAllocator(), m_buffer, m_allocation);
            GE_VK_ASSERT(vmaCreateBuffer(
                VulkanCore::GetAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));
        }
        else
        {
            VmaAllocation old_alloc  = m_allocation;
            VkBuffer      old_buffer = m_buffer;

            // alloc new buffer
            GE_VK_ASSERT(vmaCreateBuffer(
                VulkanCore::GetAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));

            // transfer
            WaitLastAction();
            TransferBuffer(old_buffer,
                           m_buffer,
                           retain_size,
                           retain_start,
                           retain_start,
                           {},
                           {m_actionCompleteSemaphore},
                           m_actionCompleteFence);
            VulkanCore::WaitForFence(m_actionCompleteFence, c_waitTimeout);

            // destroy old buffer
            vmaDestroyBuffer(VulkanCore::GetAllocator(), old_buffer, old_alloc);
        }
    }
} // namespace GE