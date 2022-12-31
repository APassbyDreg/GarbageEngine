#pragma once

#include "GE_pch.h"

#include "../RenderPass/__TestBasicMeshPass.h"
#include "../RenderPass/__TestBasicTrianglePass.h"

#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/GpuImage.h"
#include "../VulkanManager/VulkanSwapchain.h"

namespace GE
{
    class TestBasicFrameData
    {
    public:
        GpuImage                     m_image;
        VkFramebuffer                m_framebuffer;
        VkCommandPool                m_graphicsPool, m_computePool;
        std::vector<VkCommandBuffer> m_graphicsCmdBuffer, m_computeCmdBuffer;

        ~TestBasicFrameData() { DestroyFrameBuffer(); };
        void DestroyFrameBuffer()
        {
            if (m_framebuffer != VK_NULL_HANDLE)
                vkDestroyFramebuffer(VulkanCore::GetDevice(), m_framebuffer, nullptr);
        }
    };

    struct TestBasicDrawData
    {
        VkClearValue               clear_color   = {0.0f, 0.0f, 0.0f, 1.0f};
        std::shared_ptr<GpuBuffer> vertex_buffer = nullptr;
        uint                       passid;
    };

    class TestBasicRoutine
    {
    public:
        TestBasicRoutine();
        TestBasicRoutine(uint n_frames);
        ~TestBasicRoutine();

        void DrawFrame(uint         index,
                       VkSemaphore* wait_semaphores        = nullptr,
                       uint         wait_semaphore_count   = 0,
                       VkSemaphore* signal_semaphores      = nullptr,
                       uint         signal_semaphore_count = 0,
                       VkFence      fence                  = VK_NULL_HANDLE);

        void Init(uint n_frames);

        void Resize(uint width, uint height);

        inline std::shared_ptr<TestBasicFrameData> GetFrameData(uint idx) { return m_frameData[idx]; }

    private:
        TestBasicTrianglePass m_basicTrianglePass;
        TestBasicMeshPass     m_basicMeshPass;

        std::shared_ptr<AutoGpuBuffer> m_vertexBuffer;
        std::shared_ptr<AutoGpuBuffer> m_indexBuffer;

        VkExtent2D m_viewportSize = {0, 0};

        uint                                             m_frameCnt = 0;
        std::vector<std::shared_ptr<TestBasicFrameData>> m_frameData;
    };
} // namespace GE