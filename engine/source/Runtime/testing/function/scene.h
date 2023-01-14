#pragma once

#include "GE_pch.h"

#include "../UnitTest.h"
#include "Runtime/core/Log/LogSystem.h"
#include "Runtime/function/Scene/Scene.h"

namespace GE
{
    class SceneECSTestCase : public TestCase
    {
    public:
        void run() override;
    };
} // namespace GE