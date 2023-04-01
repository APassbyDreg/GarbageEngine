#pragma once

#include "GE_pch.h"

#include "../ResourceBase.h"

#include "Runtime/core/Json.h"

#include "Runtime/function/Render/VulkanManager/VulkanCreateInfoBuilder.h"
#include "vulkan/vulkan_core.h"

namespace GE
{
    /**
     * @brief 4 channel LDR image stored as byte array
     */
    class LDRImage
    {
    public:
        LDRImage(const LDRImage& other) { Copy(other); };
        LDRImage(fs::path path = "")
        {
            if (!path.empty())
            {
                Load(path);
            }
        };
        ~LDRImage() { Reset(); };

        inline byte* GetData() const { return m_data; }
        inline uint2 GetSize() const { return {m_width, m_height}; }
        inline uint  GetWidth() const { return m_width; }
        inline uint  GetHeight() const { return m_height; }
        inline uint  GetChannel() const { return m_channel; }
        inline uint  GetBufferSize() const { return m_width * m_height * 4; }
        inline bool  IsValid() const { return m_data != nullptr; }

        void Load(fs::path path);
        void Save(fs::path path);
        void Copy(const LDRImage& other);
        void Reset();

        inline LDRImage& operator=(const LDRImage& other)
        {
            Copy(other);
            return *this;
        }

        inline VkImageCreateInfo GetImageCreateInfo() const
        {
            return VkInit::GetVkImageCreateInfo(VK_IMAGE_TYPE_2D,
                                                VK_FORMAT_R8G8B8A8_UNORM,
                                                {GetWidth(), GetHeight(), 1},
                                                VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        }

    private:
        fs::path m_path;
        int      m_channel, m_width, m_height;
        byte*    m_data = nullptr;
    };

    class GE_API LDRImageResource : public Resource<LDRImage>
    {
    public:
        GE_RESOURCE_COMMON(LDRImageResource, ResourceType::IMAGE);

        inline void Invalid() override
        {
            if (m_valid)
            {
                m_valid = false;
                m_data.Reset();
            }
        }

        inline void SaveData(const LDRImage& data) override
        {
            m_data  = data;
            m_valid = true;
            Save();
        }
    };

} // namespace GE