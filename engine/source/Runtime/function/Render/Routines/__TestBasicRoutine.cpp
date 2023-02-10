#include "__TestBasicRoutine.h"
#include "vulkan/vulkan_core.h"

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

            m_basicMeshPass.Resize(width, height);
        }
    }

    void TestBasicRoutine::Init(uint n_frames)
    {
        GE_CORE_ASSERT(n_frames > 0, "[TestBasicRoutine::Init] At least one frame is needed.");
        m_frameCnt = n_frames;

        m_resourceManager.Init(n_frames);
        m_basicMeshPass.Init(n_frames);

        Resize(1280, 720);

        /* ------------------------ reserve resources ----------------------- */
        m_resourceManager.ReservePerFrameGraphicsCmdBuffer("BasicMeshPass");

        /* ------------- create default vertex and index buffer ------------- */
        {
            Vertex vertices[4]   = {{}, {}, {}, {}};
            vertices[0].position = {0.5f, 0.5f, 0.0f};
            vertices[1].position = {-0.5f, 0.5f, 0.0f};
            vertices[2].position = {-0.5f, -0.5f, 0.0f};
            vertices[3].position = {0.5f, -0.5f, 0.0f};
            uint vertices_size   = sizeof(vertices);

            VkBufferCreateInfo buffer_info =
                VkInit::GetBufferCreateInfo(vertices_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

            m_vertexBuffer = std::make_shared<AutoGpuBuffer>(buffer_info, alloc_info);
            m_vertexBuffer->Upload((byte*)vertices, vertices_size);
        }
        {
            uint32_t indices[]    = {0, 1, 2, 2, 3, 0};
            uint     indices_size = sizeof(indices);

            VkBufferCreateInfo buffer_info =
                VkInit::GetBufferCreateInfo(indices_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

            auto alloc_info = VkInit::GetAllocationCreateInfo(VMA_MEMORY_USAGE_AUTO,
                                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

            m_indexBuffer = std::make_shared<AutoGpuBuffer>(buffer_info, alloc_info);
            m_indexBuffer->Upload((byte*)indices, indices_size);
        }
    }

    void TestBasicRoutine::DrawFrame(uint                     index,
                                     std::vector<VkSemaphore> wait_semaphores,
                                     std::vector<VkSemaphore> signal_semaphores,
                                     VkFence                  fence)
    {
        uint frame_index = index % m_frameCnt;

        m_resourceManager.NewFrame(frame_index);

        VkClearValue clear_value = {};
        clear_value.color        = {0.2f, 0.2f, 0.6f, 1.0f};

        {
            RenderPassRunData     run_data  = {frame_index,
                                               m_resourceManager.GetPerFrameGraphicsCmdBuffer(frame_index, "BasicMeshPass"),
                                               wait_semaphores,
                                               signal_semaphores,
                                               fence};
            TestBasicMeshPassData pass_data = {m_vertexBuffer, m_indexBuffer, 6, m_viewportSize};

            m_basicMeshPass.Run(run_data, pass_data);
        }
    }

    VkImageView TestBasicRoutine::GetOutputImageView(uint frame_idx)
    {
        return m_resourceManager.GetPerFrameImage(frame_idx, m_basicMeshPass.FullIdentifier("color"))->GetImageView();
    }
} // namespace GE