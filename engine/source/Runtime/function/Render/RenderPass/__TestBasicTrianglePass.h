#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"

namespace GE
{
    class TestBasicTrianglePass : public GraphicsPass
    {
    public:
        TestBasicTrianglePass() {};
        ~TestBasicTrianglePass() {};

        void Run(VkRenderPassBeginInfo& rp_info, VkCommandBuffer& cmd);
        
    protected:
        virtual void InitInternal() override;
    };
} // namespace GE
