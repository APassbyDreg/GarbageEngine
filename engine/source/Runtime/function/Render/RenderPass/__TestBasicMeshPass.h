#pragma once

#include "GE_pch.h"

#include "../DataStructures/Mesh.h"
#include "../RenderPass.h"
#include "../VulkanManager/GpuBuffer.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct TestBasicMeshPushConstants
    {
        float4x4 mvp;
        float4   cameraPosWS;
        float4   debugColor;
    };

    class TestBasicMeshPass : public GraphicsPass
    {
    public:
        TestBasicMeshPass() {};
        ~TestBasicMeshPass() {};

        void Run(VkExtent2D&                viewport_size,
                 VkRenderPassBeginInfo&     rp_info,
                 VkCommandBuffer&           cmd,
                 std::shared_ptr<GpuBuffer> vertex_buffer,
                 std::shared_ptr<GpuBuffer> index_buffer,
                 uint                       vertex_cnt);
    protected:
        virtual void InitInternal() override;
    };

} // namespace GE