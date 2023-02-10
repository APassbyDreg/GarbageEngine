#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

#include "vma/vk_mem_alloc.h"

namespace GE
{
    class GE_API GpuImage
    {
    public:
        GpuImage() {};
        GpuImage(VkImageCreateInfo image_info, VkImageViewCreateInfo view_info, VmaAllocationCreateInfo alloc_info);

        ~GpuImage();

        inline VkImage     GetImage() { return m_image; }
        inline VkImageView GetImageView() { return m_imageView; }

        void Alloc(VkImageCreateInfo image_info, VkImageViewCreateInfo view_info, VmaAllocationCreateInfo alloc_info);
        void Delete();
        void Upload(void* data, size_t& size);
        void Download(void* data, size_t& size);

        inline VkImageCreateInfo       GetImageInfo() { return m_imageInfo; }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_allocInfo; }

    private:
        bool m_alloced = false;

        VkImageCreateInfo       m_imageInfo;
        VmaAllocationCreateInfo m_allocInfo;

        VkImage       m_image;
        VmaAllocation m_allocation;

        VkImageView m_imageView;
    };
} // namespace GE