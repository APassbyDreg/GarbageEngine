#include "AutoGpuBuffer.h"

namespace GE
{
    AutoGpuBuffer::~AutoGpuBuffer()
    {
        {
            std::lock_guard<std::mutex> lock(m_cvMutex);
            m_shouldExit = true;
        }
        m_cv.notify_all();
        m_updateThread.join();
    }

    void AutoGpuBuffer::Upload(byte* data, size_t size, size_t offset, bool resize)
    {
        if (resize)
        {
            Resize(size + offset, 0, m_usedSize);
        }
        m_buffer.Upload(data, size, offset, false);
        m_usedSize = size;
    }

    void AutoGpuBuffer::Download(byte* data, size_t size, size_t offset) { m_buffer.Download(data, size, offset); }

    void AutoGpuBuffer::Copy(GpuBuffer&               src_buffer,
                             size_t                   size,
                             size_t                   src_offset,
                             size_t                   dst_offset,
                             bool                     async,
                             std::vector<VkSemaphore> wait_semaphores,
                             bool                     resize_if_needed)
    {
        GE_CORE_ASSERT(dst_offset <= m_usedSize, "dst_offset out of range: offset={}, size={}", dst_offset, m_usedSize);

        if (size + dst_offset > m_currentSize)
        {
            GE_CORE_ASSERT(resize_if_needed,
                           "size out of range: copy_size={}, offset={}, full_size={}",
                           size,
                           dst_offset,
                           m_currentSize);
            Resize(size + dst_offset, 0, dst_offset);
        }
        m_buffer.Copy(src_buffer, size, src_offset, dst_offset, async, wait_semaphores, false);
    }

    void AutoGpuBuffer::Copy(AutoGpuBuffer&           src_buffer,
                             size_t                   size,
                             size_t                   src_offset,
                             size_t                   dst_offset,
                             bool                     async,
                             std::vector<VkSemaphore> wait_semaphores,
                             bool                     resize_if_needed)
    {
        Copy(src_buffer.m_buffer, size, src_offset, dst_offset, async, wait_semaphores, resize_if_needed);
    }

    void AutoGpuBuffer::Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info)
    {
        m_buffer.Alloc(buffer_info, alloc_info);
        m_currentSize = m_usedSize = buffer_info.size;
        m_tLastAdjust              = Time::CurrentTime();
    }

    void AutoGpuBuffer::Resize(size_t size_in_bytes, size_t retain_start, size_t retain_size)
    {
        size_t  new_size   = size_in_bytes;
        int64_t delta_size = size_in_bytes - m_usedSize;
        if (IsValid() && Time::CurrentTime() - m_tLastAdjust < c_tActive && delta_size > 0)
        {
            new_size += delta_size * 2;
        }

        if (new_size > m_currentSize)
        {
            m_buffer.Resize(new_size, retain_start, retain_size);
            m_currentSize = new_size;
        }
        if (size_in_bytes > m_usedSize)
        {
            m_tLastAdjust = Time::CurrentTime();
        }
        m_usedSize = size_in_bytes;
    }

    void AutoGpuBuffer::Update()
    {
        std::unique_lock<std::mutex> lk(m_cvMutex);
        bool                         exit = false;
        while (!exit)
        {
            exit = m_cv.wait_for(lk, c_tCheckInterval, [this] { return m_shouldExit; });
            GE_CORE_TRACE("AutoGpuBuffer::Update");
            if (m_buffer.IsValid() && Time::CurrentTime() - m_tLastAdjust > c_tInactive && m_currentSize > m_usedSize)
            {
                Resize(m_usedSize, 0, m_usedSize);
            }
        }
    }
} // namespace GE