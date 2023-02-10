#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"
#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/DescriptorSetLayout.h"

#include "Runtime/core/Math/Bounds.h"
#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct AABBCullPassData
    {
        float4x4 camera_vp;
    };

    struct AABBCullPassPushConstants
    {
        float4x4 camera_vp;
        uint     num_aabbs;
    };

    class AABBCullPass : public ComputePass
    {
    public:
        AABBCullPass(RenderResourceManager& resource_manager) : ComputePass(resource_manager, "AABBCull") {};
        ~AABBCullPass() {};

        void Run(RenderPassRunData& run_data, AABBCullPassData& pass_data);

    protected:
        virtual void InitInternal(uint frame_cnt) override;

    private:
        std::shared_ptr<DescriptorSetLayout> m_layout;
    };
} // namespace GE
