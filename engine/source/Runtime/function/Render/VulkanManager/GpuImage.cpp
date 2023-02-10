#include "GpuImage.h"

#include "VulkanCreateInfoBuilder.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    GpuImage::GpuImage(VkImageCreateInfo       image_info,
                       VkImageViewCreateInfo   view_info,
                       VmaAllocationCreateInfo alloc_info)
    {
        Alloc(image_info, view_info, alloc_info);
    }

    GpuImage::~GpuImage() { Delete(); }

    void GpuImage::Delete()
    {
        if (m_alloced)
        {
            vkDestroyImageView(VulkanCore::GetDevice(), m_imageView, nullptr);

            vmaDestroyImage(VulkanCore::GetAllocator(), m_image, m_allocation);

            m_alloced = false;
        }
    }

    void
    GpuImage::Alloc(VkImageCreateInfo image_info, VkImageViewCreateInfo view_info, VmaAllocationCreateInfo alloc_info)
    {
        Delete();

        m_imageInfo = image_info;
        m_allocInfo = alloc_info;

        GE_VK_ASSERT(
            vmaCreateImage(VulkanCore::GetAllocator(), &image_info, &alloc_info, &m_image, &m_allocation, nullptr));

        view_info.image = m_image;
        GE_VK_ASSERT(vkCreateImageView(VulkanCore::GetDevice(), &view_info, nullptr, &m_imageView));

        m_alloced = true;
    }

    void GpuImage::Upload(void* data, size_t& size)
    {
        GE_CORE_ASSERT(false, "[GpuImage::Upload] has not been implemented");
    }

    void GpuImage::Download(void* data, size_t& size)
    {
        GE_CORE_ASSERT(false, "[GpuImage::Download] has not been implemented");
    }
} // namespace GE