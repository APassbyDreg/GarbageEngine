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
    };
} // namespace GE
