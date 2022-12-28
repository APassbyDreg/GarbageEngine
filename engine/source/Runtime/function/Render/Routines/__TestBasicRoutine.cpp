#include "__TestBasicRoutine.h"

namespace GE
{
    TestBasicRoutine::TestBasicRoutine() {}

    TestBasicRoutine::TestBasicRoutine(uint n_frames) { Init(n_frames); }

    TestBasicRoutine::~TestBasicRoutine() {}

    void TestBasicRoutine::Resize(uint width, uint height)
    {
        if (width == 0 || height == 0)
        {
            GE_CORE_ERROR("[TestBasicRoutine::Resize] Invalid width or height");
            return;
        }
        if (width != m_viewportSize.width || height != m_viewportSize.height)
        {
            // wait all old frames to finish
            vkQueueWaitIdle(VulkanCore::GetGraphicsQueue());

            m_viewportSize.width  = width;
            m_viewportSize.height = height;

            // recreate framedata
            for (size_t i = 0; i < m_frameCnt; i++)
            {
                if (m_frameData.size() <= i)
                {
                    m_frameData.push_back(std::make_shared<TestBasicFrameData>());
                }
                auto fd = m_frameData[i];

                // (re)create image
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

                // (re)create framebuffer
                fd->DestroyFrameBuffer();
                VkFramebufferCreateInfo framebuffer_info = {};
                VkImageView             img_views[]      = {fd->m_image.GetImageView()};
                framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_info.renderPass              = m_basicTrianglePass.GetRenderPass();
                framebuffer_info.attachmentCount         = 1;
                framebuffer_info.pAttachments            = img_views;
                framebuffer_info.width                   = m_viewportSize.width;
                framebuffer_info.height                  = m_viewportSize.height;
                framebuffer_info.layers                  = 1;
                GE_VK_ASSERT(
                    vkCreateFramebuffer(VulkanCore::GetDevice(), &framebuffer_info, nullptr, &fd->m_framebuffer));
            }
        }
    }

    void TestBasicRoutine::Init(uint n_frames)
    {
        GE_CORE_ASSERT(n_frames > 0, "[TestBasicRoutine::Init] At least one frame is needed.");
        m_frameCnt = n_frames;

        m_basicTrianglePass.Init();
        m_basicMeshPass.Init();

        Resize(1280, 720);

        /* ------------------------ create frame data ----------------------- */
        for (size_t i = 0; i < n_frames; i++)
        {
            m_frameData[i]->m_graphicsPool = VulkanCore::CreateGrahicsCmdPool();
            m_frameData[i]->m_computePool  = VulkanCore::CreateComputeCmdPool();

            m_frameData[i]->m_graphicsCmdBuffer = VulkanCore::CreateCmdBuffer(m_frameData[i]->m_graphicsPool, 8);
            m_frameData[i]->m_computeCmdBuffer  = VulkanCore::CreateCmdBuffer(m_frameData[i]->m_computePool, 8);
        }

        /* ------------- create default vertex and index buffer ------------- */
        {
            Vertex vertices[4]   = {{}, {}, {}, {}};
            vertices[0].position = {0.5f, 0.5f, 0.0f};
            vertices[1].position = {-0.5f, 0.5f, 0.0f};
            vertices[2].position = {-0.5f, -0.5f, 0.0f};
            vertices[3].position = {0.5f, -0.5f, 0.0f};
            uint vertices_size   = sizeof(vertices);

            VkBufferCreateInfo buffer_info = {};
            buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.size               = vertices_size;
            buffer_info.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

            m_vertexBuffer = std::make_shared<GpuBuffer>(buffer_info, alloc_info);
            m_vertexBuffer->Upload(vertices, vertices_size);
        }
        {
            uint32_t indices[]    = {0, 1, 2, 2, 3, 0};
            uint     indices_size = sizeof(indices);

            VkBufferCreateInfo buffer_info = {};
            buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.size               = indices_size;
            buffer_info.usage              = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

            m_indexBuffer = std::make_shared<GpuBuffer>(buffer_info, alloc_info);
            m_indexBuffer->Upload(indices, indices_size);
        }
    }

    void TestBasicRoutine::DrawFrame(uint         index,
                                     VkSemaphore* wait_semaphores,
                                     uint         wait_semaphore_count,
                                     VkSemaphore* signal_semaphores,
                                     uint         signal_semaphore_count,
                                     VkFence      fence)
    {
        std::shared_ptr<TestBasicFrameData> fd = m_frameData[index];

        VulkanCore::ResetCmdPool(fd->m_graphicsPool);
        VulkanCore::ResetCmdPool(fd->m_computePool);

        int          passid      = 1;
        VkClearValue clear_value = {};
        clear_value.color        = {0.2f, 0.2f, 0.6f, 1.0f};
        if (passid == 0)
        {
            VkRenderPassBeginInfo info = {};
            info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass            = m_basicTrianglePass.GetRenderPass();
            info.framebuffer           = fd->m_framebuffer;
            info.renderArea.offset     = {0, 0};
            info.renderArea.extent     = m_viewportSize;
            info.clearValueCount       = 1;
            info.pClearValues          = &clear_value;

            m_basicTrianglePass.Run(info, fd->m_graphicsCmdBuffer[0]);
        }
        else if (passid == 1)
        {
            VkRenderPassBeginInfo info = {};
            info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass            = m_basicMeshPass.GetRenderPass();
            info.framebuffer           = fd->m_framebuffer;
            info.renderArea.offset     = {0, 0};
            info.renderArea.extent     = m_viewportSize;
            info.clearValueCount       = 1;
            info.pClearValues          = &clear_value;

            m_basicMeshPass.Run(m_viewportSize, info, fd->m_graphicsCmdBuffer[0], m_vertexBuffer, m_indexBuffer, 6);
        }

        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = wait_semaphore_count;
            info.pWaitSemaphores            = wait_semaphores;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &fd->m_graphicsCmdBuffer[0];
            info.signalSemaphoreCount       = signal_semaphore_count;
            info.pSignalSemaphores          = signal_semaphores;

            VulkanCore::SubmitToGraphicsQueue(info, fence);
        }
    }
} // namespace GE