#pragma once

#include "GE_pch.h"

#include "../Mesh/TriangleMesh.h"
#include "../RenderPass.h"
#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/FrameBuffer.h"

#include "Runtime/core/Math/Math.h"
#include <memory>
#include <vector>

namespace GE
{
    struct TestBasicMeshPushConstants
    {
        float4x4 mvp;
        float4   cameraPosWS;
        float4   debugColor;
    };

    struct TestBasicMeshPassData
    {
        std::shared_ptr<AutoGpuBuffer> vertex_buffer;
        std::shared_ptr<AutoGpuBuffer> index_buffer;
        uint                           vertex_cnt;
        VkExtent2D&                    viewport_size;
    };

    class TestBasicMeshPass : public GraphicsPass
    {
    public:
        TestBasicMeshPass(RenderResourceManager& resource_manager) :
            GraphicsPass(resource_manager, "Test/BasicMesh") {};
        ~TestBasicMeshPass() {};

        void Resize(uint width, uint height) override;

        void Run(RenderPassRunData run_data, TestBasicMeshPassData pass_data);

    protected:
        virtual void InitInternal(uint frame_cnt) override;

        std::vector<std::shared_ptr<FrameBuffer>> m_frameBuffers = {};
    };

} // namespace GE