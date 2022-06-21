#pragma once

#include "GE_pch.h"

#include "../UnitTest.h"
#include "Runtime/function/Render/VulkanManager/ShaderManager.h"
#include "Runtime/resource/Managers/CacheManager.h"

namespace GE
{
    class CacheManagerTestCase : public TestCase
    {
    public:
        void run() override;
    };

    class ShaderManagerTestCase : public TestCase
    {
    public:
        void run() override;
    };
} // namespace GE