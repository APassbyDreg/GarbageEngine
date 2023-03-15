#pragma once

#include "GE_pch.h"

#include "Runtime/core/IntervalJob.h"
#include "Runtime/core/Time.h"

#include "GpuBuffer.h"

namespace GE
{
    class GE_API AutoGpuBuffer
    {
        const Time::Seconds c_tCheckInterval = Time::Seconds(2);
        // If the buffer the size is not enlarged for about 5s, its size will be reduced to the used size, which is
        // implemented by a guard thread.
        const Time::Seconds c_tInactive = Time::Seconds(5);
        // If the buffer is enlarged less than 50ms ago, it will be given more space.
        const Time::Seconds c_tActive = Time::Miliseconds(100);

    public:
        ~AutoGpuBuffer() {}
        AutoGpuBuffer() { m_tLastAdjust = Time::CurrentTime(); }
        AutoGpuBuffer(AutoGpuBuffer& src)
        {
            m_buffer.Copy(src.m_buffer);
            m_currentSize  = src.m_currentSize;
            m_usedSize     = src.m_usedSize;
            m_minSize      = src.m_minSize;
            m_tLastAdjust  = Time::CurrentTime();
        }
        AutoGpuBuffer(AutoGpuBuffer&& src) : m_buffer(std::move(src.m_buffer))
        {
            // copy info and start update thread
            m_currentSize  = src.m_currentSize;
            m_usedSize     = src.m_usedSize;
            m_minSize      = src.m_minSize;
            m_tLastAdjust  = Time::CurrentTime();
        }
        AutoGpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info, size_t min_size = 16ull)
        {
            m_currentSize = m_usedSize = buffer_info.size;
            m_minSize                  = min_size;
            m_tLastAdjust              = Time::CurrentTime();
            Alloc(buffer_info, alloc_info);
        }

        void           Upload(byte* data, size_t size, size_t offset = 0, bool resize = true);
        void           Download(byte* data, size_t size, size_t offset = 0);
        void           Copy(GpuBuffer& src_buffer,
                            size_t     size             = 0,
                            size_t     src_offset       = 0,
                            size_t     dst_offset       = 0,
                            bool       resize_if_needed = true);
        void           Copy(AutoGpuBuffer& src_buffer,
                            size_t         size             = 0,
                            size_t         src_offset       = 0,
                            size_t         dst_offset       = 0,
                            bool           resize_if_needed = true);
        void           Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);
        void           Resize(size_t sizeInBytes, size_t retain_start = 0, size_t retain_size = 0, bool force = false);

        template<typename T>
        void UploadAs(std::vector<T>& data, size_t offset = 0, bool resize = true)
        {
            const size_t size_in_bytes   = data.size() * sizeof(T);
            const size_t offset_in_bytes = offset * sizeof(T);
            if (resize)
            {
                Resize(size_in_bytes + offset_in_bytes, 0, m_usedSize);
            }
            m_buffer.Upload((byte*)data.data(), size_in_bytes, offset, false);
            m_usedSize = size_in_bytes;
        }
        template<typename T>
        std::vector<T> DownloadAs(size_t count, size_t offset = 0)
        {
            return m_buffer.DownloadAs<T>(count, offset);
        }

        inline size_t                  GetSize() { return m_usedSize; }
        inline size_t                  GetWholeSize() { return m_currentSize; }
        inline VkBuffer                GetBuffer() { return m_buffer.GetBuffer(); }
        inline VkBufferCreateInfo      GetBufferInfo() { return m_buffer.GetBufferInfo(); }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_buffer.GetAllocInfo(); }
        inline bool                    IsValid() { return m_buffer.IsValid(); }

    private:
        GpuBuffer m_buffer;

        size_t          m_currentSize, m_usedSize, m_minSize;
        Time::TimeStamp m_tLastAdjust;

        // Time weighted average size
        const double    c_sizeWeightDecayPerSec = 0.25;
        double          m_avgSize               = 0.0;
        Time::TimeStamp m_tLastRecord;
        void            UpdateSize();
    };
} // namespace GE