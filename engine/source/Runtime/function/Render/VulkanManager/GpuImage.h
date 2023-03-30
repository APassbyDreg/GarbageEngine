#pragma once

#include "GE_pch.h"

#include "VulkanCore.h"

#include "Runtime/core/Utils/ContainerUtils.h"

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    class GE_API GpuImage
    {
    public:
        GpuImage() {};
        GpuImage(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info);

        ~GpuImage();

        inline VkImage     GetImage() { return m_image; }
        inline VkImageView GetImageView(VkImageViewCreateInfo info, bool create = false)
        {
            auto key = __HashViewInfo(info);
            if (create && !StdUtils::Exists(m_imageViews, key))
            {
                AddImageView(info);
            }
            GE_CORE_ASSERT(StdUtils::Exists(m_imageViews, key), "ImageView not found");
            return m_imageViews[key];
        }

        void Alloc(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info);
        void Delete();
        void AddImageView(VkImageViewCreateInfo info);

        inline VkImageCreateInfo       GetImageInfo() { return m_imageInfo; }
        inline VmaAllocationCreateInfo GetAllocInfo() { return m_allocInfo; }

        inline VkExtent3D GetExtent() { return m_imageInfo.extent; }
        inline bool       IsValid() { return m_alloced; }

    private:
        bool m_alloced = false;

        VkImageCreateInfo       m_imageInfo;
        VmaAllocationCreateInfo m_allocInfo;

        VkImage       m_image;
        VmaAllocation m_allocation;

        std::map<size_t, VkImageView> m_imageViews;

        size_t __HashViewInfo(VkImageViewCreateInfo info);
    };
} // namespace GE