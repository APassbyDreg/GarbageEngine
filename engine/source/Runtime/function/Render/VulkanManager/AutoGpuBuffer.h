#pragma once

#include "GE_pch.h"

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
        const Time::Seconds c_tActive = Time::Miliseconds(50);

    public:
        ~AutoGpuBuffer();
        AutoGpuBuffer()
        {
            m_tLastAdjust  = Time::CurrentTime();
            m_updateThread = std::thread(&AutoGpuBuffer::Update, this);
        }
        AutoGpuBuffer(AutoGpuBuffer& src)
        {
            m_buffer.Copy(src.m_buffer);
            m_currentSize  = src.m_currentSize;
            m_usedSize     = src.m_usedSize;
            m_tLastAdjust  = Time::CurrentTime();
            m_updateThread = std::thread(&AutoGpuBuffer::Update, this);
        }
        AutoGpuBuffer(AutoGpuBuffer&& src) : m_buffer(std::move(src.m_buffer))
        {
            // exit the update thread
            {
                std::lock_guard<std::mutex> lock(src.m_cvMutex);
                src.m_shouldExit = true;
            }
            src.m_cv.notify_all();
            src.m_updateThread.join();

            // copy info and start a new update thread
            m_currentSize  = src.m_currentSize;
            m_usedSize     = src.m_usedSize;
            m_tLastAdjust  = Time::CurrentTime();
            m_updateThread = std::thread(&AutoGpuBuffer::Update, this);
        }
        AutoGpuBuffer(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info)
        {
            Alloc(buffer_info, alloc_info);
            m_currentSize = m_usedSize = buffer_info.size;
            m_tLastAdjust              = Time::CurrentTime();
            m_updateThread             = std::thread(&AutoGpuBuffer::Update, this);
        }

        void           Upload(byte* data, size_t size, size_t offset = 0, bool resize = true);
        void           Download(byte* data, size_t size, size_t offset = 0);
        void           Copy(GpuBuffer&               src_buffer,
                            size_t                   size             = 0,
                            size_t                   src_offset       = 0,
                            size_t                   dst_offset       = 0,
                            bool                     async            = false,
                            std::vector<VkSemaphore> wait_semaphores  = {},
                            bool                     resize_if_needed = true);
        void           Copy(AutoGpuBuffer&           src_buffer,
                            size_t                   size             = 0,
                            size_t                   src_offset       = 0,
                            size_t                   dst_offset       = 0,
                            bool                     async            = false,
                            std::vector<VkSemaphore> wait_semaphores  = {},
                            bool                     resize_if_needed = true);
        void           Alloc(VkBufferCreateInfo buffer_info, VmaAllocationCreateInfo alloc_info);
        void           Resize(size_t sizeInBytes, size_t retain_start = 0, size_t retain_size = 0);

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
        inline VkBuffer                GetBuffer() { return m_buffer.GetBuffer(); }
        inline VkSemaphore             GetSemaphore() { return m_buffer.GetSemaphore(); }
        inline VkBufferCreateInfo      GetBufferInfo() { return m_buffer.GetBufferInfo(); }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_buffer.GetAllocInfo(); }
        inline bool                    IsValid() { return m_buffer.IsValid(); }

        void Update();

    private:
        GpuBuffer m_buffer;

        size_t m_currentSize, m_usedSize;

        Time::TimeStamp m_tLastAdjust;

        std::thread             m_updateThread;
        std::condition_variable m_cv;
        std::mutex              m_cvMutex;
        bool                    m_shouldExit = false;
    };
} // namespace GE