#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"
#include "../VulkanManager/AutoGpuBuffer.h"

#include "Runtime/core/Math/Bounds.h"
#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct AABBCullPassData
    {
        float4x4 camera_vp;
        uint32   num_aabbs;
    };

    class AABBCullPass : public ComputePass
    {
        struct FrameData
        {
            std::shared_ptr<AutoGpuBuffer> aabb_buffer, result_buffer;
            VkDescriptorSet            desc_set;
        };

    public:
        AABBCullPass() {};
        ~AABBCullPass() {};

        void Run(RenderPassRunData& run_data, AABBCullPassData& pass_data);

    protected:
        virtual void InitInternal(uint frame_cnt) override;

    private:
        std::vector<FrameData> m_frameData;
    };
} // namespace GE
