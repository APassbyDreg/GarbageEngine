#pragma once

#include "GE_pch.h"

#include "../DataStructures/Mesh.h"
#include "../RenderPass.h"
#include "../VulkanManager/ShaderManager.h"

namespace GE
{
    class TestBasicMeshPass : public RenderPass
    {
    public:
        TestBasicMeshPass() {};
        ~TestBasicMeshPass() {};

        void Init();
    };

} // namespace GE