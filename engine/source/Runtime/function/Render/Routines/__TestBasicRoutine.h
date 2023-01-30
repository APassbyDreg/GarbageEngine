#pragma once

#include "GE_pch.h"

#include "../RenderPass/__TestBasicMeshPass.h"

#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/GpuImage.h"
#include "../VulkanManager/VulkanSwapchain.h"

namespace GE
{
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

        VkImageView GetOutputImageView(uint frame_idx);

    private:
        RenderResourceManager m_resourceManager;
        TestBasicMeshPass     m_basicMeshPass {m_resourceManager};

        std::shared_ptr<AutoGpuBuffer> m_vertexBuffer;
        std::shared_ptr<AutoGpuBuffer> m_indexBuffer;

        VkExtent2D m_viewportSize = {0, 0};

        uint m_frameCnt = 0;
    };
} // namespace GE