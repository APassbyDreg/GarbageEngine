#pragma once

#include "GE_pch.h"

#include "../DataStructures/Mesh.h"
#include "../RenderPass.h"
#include "../VulkanManager/ShaderManager.h"

#include "Runtime/core/math/math.h"

namespace GE
{
    struct TestBasicMeshPushConstants
    {
        float4x4 mvp;
        float4   cameraPosWS;
        float4   debugColor;
    };

    class TestBasicMeshPass : public RenderPass
    {
    public:
        TestBasicMeshPass() {};
        ~TestBasicMeshPass() {};

        void Init();
    };

} // namespace GE