#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Singleton.h"
#include "Runtime/core/Math/Bounds.h"

namespace GE
{
    class Entity;

    class EntityAABBLogic : public Singleton<EntityAABBLogic>
    {
    private:
        struct AABBCacheValue
        {
            Bounds3f value;
            uint     version;
        };

    public:
        Bounds3f GetAABB(Entity& e);

    private:
        std::map<int, AABBCacheValue> m_AABBCache;
    };

} // namespace GE