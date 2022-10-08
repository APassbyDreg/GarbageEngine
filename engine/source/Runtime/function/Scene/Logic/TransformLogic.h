#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"

#include "../Entity.h"

namespace GE
{
    class TransformLogic : public Singleton<TransformLogic>
    {
    private:
        struct MatCacheValue
        {
            float4x4 value;
            uint     version;
        };

    public:
        float4x4 GetAbsoluteTransformMatrix(Entity& e);

    private:
        std::map<int, MatCacheValue> m_AbsTransMatrixCache;
    };

} // namespace GE