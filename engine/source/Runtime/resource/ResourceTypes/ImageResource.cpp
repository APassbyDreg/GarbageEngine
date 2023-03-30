#include "ImageResource.h"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"
#include <vcruntime_string.h>

namespace GE
{
    void LDRImage::Copy(const LDRImage& other)
    {
        if (m_data != nullptr)
        {
            GE_CORE_WARN("Trying to copy {} to an loaded image", other.m_path.string());
            Reset();
        }
        m_path    = other.m_path;
        m_width   = other.m_width;
        m_height  = other.m_height;
        m_channel = other.m_channel;
        m_data    = new byte[GetBufferSize()];
        memcpy(m_data, other.m_data, GetBufferSize());
    }

    void LDRImage::Load(fs::path path)
    {
        if (m_data != nullptr)
        {
            GE_CORE_WARN("Trying to load {} to an loaded image", path.string());
            Reset();
        }

        m_data = stbi_load(path.string().c_str(), &m_width, &m_height, &m_channel, STBI_rgb_alpha);

        if (m_data == nullptr)
        {
            GE_CORE_ERROR("Failed to load image: {}", path.string());
            return;
        }

        m_path = path;
    }

    void LDRImage::Save(fs::path path)
    {
        m_path = path;

        if (m_data == nullptr)
        {
            GE_CORE_ERROR("Trying to save an invalid image to {}", path.string());
            return;
        }

        if (path.extension() == "png")
        {
            stbi_write_png(path.string().c_str(), m_width, m_height, m_channel, m_data, m_width * m_channel);
        }
        else if (path.extension() == "jpg")
        {
            stbi_write_jpg(path.string().c_str(), m_width, m_height, m_channel, m_data, 100);
        }
        else if (path.extension() == "bmp" || path.extension() == "jpeg")
        {
            stbi_write_bmp(path.string().c_str(), m_width, m_height, m_channel, m_data);
        }
        else
        {
            GE_CORE_ERROR("Unsupported image format: {}", path.extension().string());
        }
    }

    void LDRImage::Reset()
    {
        if (m_data != nullptr)
        {
            stbi_image_free(m_data);
        }
        m_data    = nullptr;
        m_channel = 0;
        m_width   = 0;
        m_height  = 0;
        m_path    = "";
    }

    void LDRImageResource::Load()
    {
        m_data.Load(m_filePath);

        if (!m_data.IsValid())
        {
            GE_CORE_ERROR("Failed to load image: {}", m_filePath.string());
            return;
        }

        m_valid = true;
    }

    void LDRImageResource::Save() { m_data.Save(m_filePath); }
} // namespace GE