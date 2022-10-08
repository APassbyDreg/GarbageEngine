#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "../Components/Camera.h"
#include "../Components/Transform.h"
#include "../Entity.h"

namespace GE
{
    class CameraLogic : public Singleton<CameraLogic>
    {
    private:
        struct MatCacheValue
        {
            float4x4 value;
            uint     version;
        };

    public:
        float4x4 GetVMatrix(const Entity& e);
        float4x4 GetPMatrix(const Entity& e);
        float4x4 GetVPMatrix(const Entity& e);

    private:
        std::map<int, MatCacheValue> m_VMatrixCache, m_PMatrixCache, m_VPMatrixCache;

        // Camera looks at z direction with y as up in local coord
        const float4x4 m_defaultLookAt = glm::lookAt(float3(0, 0, 0), float3(0, 0, 1), float3(0, 1, 0));
    };

} // namespace GE