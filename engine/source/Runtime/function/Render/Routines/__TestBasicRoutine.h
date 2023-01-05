#pragma once

#include "GE_pch.h"

#include "../RenderPass/__TestBasicMeshPass.h"

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

    class TestBasicRoutine
    {
    public:
        TestBasicRoutine();
        TestBasicRoutine(uint n_frames);
        ~TestBasicRoutine();

        void DrawFrame(uint                     index,
                       std::vector<VkSemaphore> wait_semaphores   = {},
                       std::vector<VkSemaphore> signal_semaphores = {},
                       VkFence                  fence             = VK_NULL_HANDLE);

        void Init(uint n_frames);

        void Resize(uint width, uint height);

        inline std::shared_ptr<TestBasicFrameData> GetFrameData(uint idx) { return m_frameData[idx]; }

    private:
        TestBasicMeshPass     m_basicMeshPass;

        std::shared_ptr<AutoGpuBuffer> m_vertexBuffer;
        std::shared_ptr<AutoGpuBuffer> m_indexBuffer;

        VkExtent2D m_viewportSize = {0, 0};

        uint                                             m_frameCnt = 0;
        std::vector<std::shared_ptr<TestBasicFrameData>> m_frameData;
    };
} // namespace GE