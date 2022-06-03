#pragma once

#include "GE_pch.h"

#include "../UnitTest.h"
#include "resource/Managers/CacheManager.h"
#include "function/Render/VulkanManager/ShaderManager.h"

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