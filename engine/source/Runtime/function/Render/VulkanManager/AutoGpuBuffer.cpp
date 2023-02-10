#include "AutoGpuBuffer.h"
#include <vcruntime.h>

namespace GE
{
    void AutoGpuBuffer::Upload(byte* data, size_t size, size_t offset, bool resize)
    {
        if (resize)
        {
            Resize(size + offset, 0, m_usedSize);
        }
        m_buffer.Upload(data, size, offset, false);
        m_usedSize = size;

        UpdateSize();
    }

    void AutoGpuBuffer::Download(byte* data, size_t size, size_t offset)
    {
        m_buffer.Download(data, size, offset);

        UpdateSize();
    }

    void
    AutoGpuBuffer::Copy(GpuBuffer& src_buffer, size_t size, size_t src_offset, size_t dst_offset, bool resize_if_needed)
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
        m_buffer.Copy(src_buffer, size, src_offset, dst_offset, false);

        UpdateSize();
    }

    void AutoGpuBuffer::Copy(AutoGpuBuffer& src_buffer,
                             size_t         size,
                             size_t         src_offset,
                             size_t         dst_offset,
                             bool           resize_if_needed)
    {
        Copy(src_buffer.m_buffer, size, src_offset, dst_offset, resize_if_needed);
    }

    void AutoGpuBuffer::Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info)
    {
        m_buffer.Alloc(buffer_info, alloc_info);
        m_currentSize = m_usedSize = buffer_info.size;
        m_tLastAdjust              = Time::CurrentTime();

        UpdateSize();
    }

    void AutoGpuBuffer::Resize(size_t target_size, size_t retain_start, size_t retain_size, bool force)
    {
        if (force)
        {
            m_buffer.Resize(target_size, retain_start, retain_size);
            m_currentSize = target_size;
            m_tLastAdjust = Time::CurrentTime();
        }
        else
        {
            size_t new_size   = target_size;
            int64  delta_size = target_size - m_usedSize;
            if (IsValid() && Time::CurrentTime() - m_tLastAdjust < c_tActive && delta_size > 0)
            {
                new_size = Max<size_t>(target_size + delta_size * 4, round(1.4 * m_usedSize));
            }

            if (new_size > m_currentSize)
            {
                m_buffer.Resize(new_size, retain_start, retain_size);
                m_currentSize = new_size;
                m_tLastAdjust = Time::CurrentTime();
            }
            m_usedSize = target_size;

            UpdateSize();
        }
    }

    void AutoGpuBuffer::UpdateSize()
    {
        Time::TimeStamp t = Time::CurrentTime();

        // record avg size
        if (m_avgSize == 0)
        {
            m_avgSize     = m_usedSize;
            m_tLastRecord = t;
        }
        else
        {
            double dt     = Time::ToSeconds(t - m_tLastRecord);
            double weight = pow(c_sizeWeightDecayPerSec, dt);
            m_avgSize     = m_avgSize * weight + m_usedSize * (1.0 - weight);
            m_tLastRecord = t;
        }

        // change size if needed
        if (t - m_tLastAdjust > c_tInactive && m_avgSize < m_currentSize * 0.7)
        {
            size_t new_size = Max<size_t>(RoundTo<4>((m_avgSize + m_currentSize) * 0.5), m_usedSize);
            Resize(new_size, 0, m_usedSize, true);
        }
    }
} // namespace GE