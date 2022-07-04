#pragma once

#include "GE_pch.h"

#include "../Passes/__TestBasicTrianglePass.h"

#include "../VulkanManager/GpuImage.h"
#include "../VulkanManager/VulkanSwapchain.h"

namespace GE
{
    class TestBasicFrameData
    {
    public:
        GpuImage      m_image;
        VkFramebuffer m_framebuffer;
    };

    struct TestBasicDrawData
    {
        VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    };

    class TestBasicRoutine
    {
    public:
        TestBasicRoutine();
        TestBasicRoutine(uint n_frames);
        ~TestBasicRoutine();

        void DrawFrame(TestBasicDrawData& draw_data, uint index, VkCommandBuffer cmd);

        void Init(uint n_frames);

        inline std::shared_ptr<TestBasicFrameData> GetFrameData(uint idx) { return m_frameData[idx]; }

    private:
        TestBasicTrianglePass m_basicTrianglePass;

        VkExtent2D m_viewportSize;

        std::vector<std::shared_ptr<TestBasicFrameData>> m_frameData;
    };
} // namespace GE