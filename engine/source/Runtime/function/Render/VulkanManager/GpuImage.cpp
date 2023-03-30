#include "GpuImage.h"

#include "VulkanCreateInfoBuilder.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    GpuImage::GpuImage(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info)
    {
        Alloc(image_info, alloc_info);
    }

    GpuImage::~GpuImage() { Delete(); }

    void GpuImage::Delete()
    {
        if (m_alloced)
        {
            for (auto& [info, view] : m_imageViews)
                vkDestroyImageView(VulkanCore::GetDevice(), view, nullptr);

            vmaDestroyImage(VulkanCore::GetAllocator(), m_image, m_allocation);

            m_alloced = false;
        }
    }

    void GpuImage::Alloc(VkImageCreateInfo image_info, VmaAllocationCreateInfo alloc_info)
    {
        Delete();

        m_imageInfo = image_info;
        m_allocInfo = alloc_info;

        GE_VK_ASSERT(
            vmaCreateImage(VulkanCore::GetAllocator(), &image_info, &alloc_info, &m_image, &m_allocation, nullptr));

        m_alloced = true;
    }

    void GpuImage::AddImageView(VkImageViewCreateInfo info)
    {
        VkImageView view;
        info.image = m_image;
        GE_VK_ASSERT(vkCreateImageView(VulkanCore::GetDevice(), &info, nullptr, &view));
        m_imageViews[__HashViewInfo(info)] = view;
    }

    size_t GpuImage::__HashViewInfo(VkImageViewCreateInfo info)
    {
        // TODO: improve efficiency

        // sync invariant values
        info.pNext = nullptr;
        info.image = nullptr;

        size_t  hash    = info.flags;          // initialize with flags
        auto    hash_fn = std::hash<uint64>(); // hash 8 bytes at once
        uint64* vals    = reinterpret_cast<uint64*>(&info);

        for (uint i = offsetof(VkImageViewCreateInfo, viewType) / sizeof(uint64);  // start from viewType
             i * sizeof(uint64) + sizeof(uint64) <= sizeof(VkImageViewCreateInfo); // ensure at least one more uint64
             i++)
        {
            hash = hash ^ hash_fn(vals[i]);
        }
        return hash;
    }
} // namespace GE