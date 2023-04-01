#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Random.h"
#include "Runtime/function/Scene/Manager/CameraManager.h"

#include "../VulkanManager/GpuBuffer.h"
#include "../VulkanManager/GpuImage.h"

#include "../RenderResource.h"

#include "../RenderPass/AABBCullPass.h"
#include "../RenderPass/ColorMappingPass.h"
#include "../RenderPass/ForwardShadingPass.h"

#include "../Shared/ViewUniform.h"

#include "../Manager/PerSceneDataManager.h"
#include "../Manager/PerViewDataManager.h"

namespace GE
{
    class BuiltinRenderRoutineFrameData
    {
    public:
        GpuImage                     m_image;
        VkFramebuffer                m_framebuffer;
        VkCommandPool                m_graphicsPool, m_computePool;
        VkDescriptorPool             m_descriptorSet;
        std::vector<VkCommandBuffer> m_graphicsCmdBuffer, m_computeCmdBuffer;

        ~BuiltinRenderRoutineFrameData() { DestroyFrameBuffer(); };

        void DestroyFrameBuffer()
        {
            if (m_framebuffer != VK_NULL_HANDLE)
                vkDestroyFramebuffer(VulkanCore::GetDevice(), m_framebuffer, nullptr);
        }
    };

    class BuiltinRenderRoutine
    {
    public:
        BuiltinRenderRoutine();
        BuiltinRenderRoutine(uint n_frames);
        ~BuiltinRenderRoutine();

        void DrawFrame(uint                     index,
                       std::vector<VkSemaphore> wait_semaphores   = {},
                       std::vector<VkSemaphore> signal_semaphores = {},
                       VkFence                  fence             = VK_NULL_HANDLE);

        void Init(uint n_frames);

        void Resize(uint width, uint height);

        VkImageView GetOutputImageView(uint frame_idx);

    private:
        RenderResourceManager m_renderResourceManager;

        /* ----------------------------- states ----------------------------- */
        uint                                                        m_frameCnt = 0;
        std::vector<std::shared_ptr<BuiltinRenderRoutineFrameData>> m_frameData;
        VkExtent2D      m_viewportSize  = {0, 0};
        uint            m_frameRendered = 0;
        Time::TimeStamp m_tInit, m_tLastFrame;
        RandomEngine    m_rand;
        CameraInfo      m_currCamInfo, m_prevCamInfo;

        /* ----------------------------- passes ----------------------------- */
        OpaqueForwardShadingPass m_opaqueForwardShadingPass {m_renderResourceManager};
        ColorMappingPass         m_colorMappingPass {m_renderResourceManager};

        /* ---------------------------- managers ---------------------------- */
        PerSceneDataManager m_perSceneDataManager {m_renderResourceManager};
        PerViewDataManager  m_perViewDataManager {m_renderResourceManager};

    private:
        ViewUniform GetBaseViewUniform(Time::TimeStamp t);
    };
} // namespace GE