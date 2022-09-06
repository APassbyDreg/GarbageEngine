#include "GpuImage.h"

#include "VulkanCreateInfoBuilder.h"

namespace GE
{
    GpuImage::GpuImage(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info)
    {
        Alloc(image_info, alloc_info);
    }

    GpuImage::~GpuImage()
    {
        if (m_alloced)
        {
            vkDestroyImageView(VulkanCore::GetVkDevice(), m_imageView, nullptr);

            vmaDestroyImage(VulkanCore::GetAllocator(), m_image, m_allocation);

            m_alloced = false;
        }
    }

    void GpuImage::Alloc(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info)
    {
        m_imageInfo = image_info;
        m_allocInfo = alloc_info;

        GE_VK_ASSERT(
            vmaCreateImage(VulkanCore::GetAllocator(), &image_info, &alloc_info, &m_image, &m_allocation, nullptr));

        VkImageViewCreateInfo info = VkInit::GetVkImageViewCreateInfo(m_image, image_info);
        GE_VK_ASSERT(vkCreateImageView(VulkanCore::GetVkDevice(), &info, nullptr, &m_imageView));

        m_alloced = true;
    }

    void GpuImage::Upload(void* data, size_t& size) {}

    void GpuImage::Download(void* data, size_t& size) {}
} // namespace GE