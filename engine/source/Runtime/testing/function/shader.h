#pragma once

#include "GE_pch.h"

#include "../UnitTest.h"

#include "Runtime/function/Render/ShaderManager/GLSLCompiler.h"
#include "Runtime/function/Render/ShaderManager/HLSLCompiler.h"

namespace GE
{
    class ShaderTestCase : public TestCase
    {
    public:
        void run() override;
    };
} // namespace GE