#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"

namespace GE
{
    class TestBasicTrianglePass : public RenderPass
    {
    public:
        TestBasicTrianglePass() {};
        ~TestBasicTrianglePass() {};

        void Init();

        void Run(VkRenderPassBeginInfo& rp_info, VkCommandBuffer& cmd);
    };
} // namespace GE
