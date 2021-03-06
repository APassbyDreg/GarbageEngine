#include "__TestBasicRoutine.h"

namespace GE
{
    TestBasicRoutine::TestBasicRoutine() {}

    TestBasicRoutine::TestBasicRoutine(uint n_frames) { Init(n_frames); }

    TestBasicRoutine::~TestBasicRoutine()
    {
        for (size_t i = 0; i < m_frameData.size(); i++)
        {
            vkDestroyFramebuffer(VulkanCore::GetVkDevice(), m_frameData[i]->m_framebuffer, nullptr);
        }
    }

    void TestBasicRoutine::Init(uint n_frames)
    {
        GE_CORE_ASSERT(n_frames > 0, "[TestBasicRoutine::Init] At least one frame is needed.");

        m_basicTrianglePass.Init();

        m_viewportSize = {1280, 720};

        for (size_t i = 0; i < n_frames; i++)
        {
            std::shared_ptr<TestBasicFrameData> fd = std::make_shared<TestBasicFrameData>();

            VkImageCreateInfo image_info       = {};
            image_info.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType               = VK_IMAGE_TYPE_2D;
            image_info.extent.width            = m_viewportSize.width;
            image_info.extent.height           = m_viewportSize.height;
            image_info.extent.depth            = 1;
            image_info.format                  = VK_FORMAT_R8G8B8A8_UNORM;
            image_info.mipLevels               = 1;
            image_info.arrayLayers             = 1;
            image_info.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage                   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_info.samples                 = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling                  = VK_IMAGE_TILING_OPTIMAL;
            image_info.sharingMode             = VK_SHARING_MODE_EXCLUSIVE;
            VmaAllocationCreateInfo alloc_info = {};
            fd->m_image.Alloc(image_info, alloc_info);

            VkFramebufferCreateInfo framebuffer_info = {};
            VkImageView             img_views[]      = {fd->m_image.GetImageView()};
            framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass              = m_basicTrianglePass.GetRenderPass();
            framebuffer_info.attachmentCount         = 1;
            framebuffer_info.pAttachments            = img_views;
            framebuffer_info.width                   = m_viewportSize.width;
            framebuffer_info.height                  = m_viewportSize.height;
            framebuffer_info.layers                  = 1;
            VK_CHECK(vkCreateFramebuffer(VulkanCore::GetVkDevice(), &framebuffer_info, nullptr, &fd->m_framebuffer));
            m_frameData.emplace_back(fd);
        }
    }

    void TestBasicRoutine::DrawFrame(TestBasicDrawData& draw_data, uint index, VkCommandBuffer cmd)
    {
        std::shared_ptr<TestBasicFrameData> fd = m_frameData[index];

        {
            VkRenderPassBeginInfo info = {};
            info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass            = m_basicTrianglePass.GetRenderPass();
            info.framebuffer           = fd->m_framebuffer;
            info.renderArea.offset     = {0, 0};
            info.renderArea.extent     = m_viewportSize;
            info.clearValueCount       = 1;
            info.pClearValues          = &draw_data.clear_color;
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_basicTrianglePass.GetPipeline());

        vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdEndRenderPass(cmd);
    }
} // namespace GE