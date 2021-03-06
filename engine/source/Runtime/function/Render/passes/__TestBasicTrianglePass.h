#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"

#include "../VulkanManager/ShaderManager.h"

namespace GE
{
    class TestBasicTrianglePass : public RenderPass
    {
    public:
        TestBasicTrianglePass();
        ~TestBasicTrianglePass();

        void Init();
    };
} // namespace GE
