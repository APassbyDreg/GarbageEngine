#include "Transform.h"

#include "../Components/Transform.h"

namespace GE
{
    float4x4 TransformLogic::GetAbsoluteTransformMatrix(Entity& e)
    {
        uint version = e.GetVersion();
        int  eid     = e.GetEntityID();
        // check cache
        auto v_cache = m_AbsTransMatrixCache.find(eid);
        if (v_cache != m_AbsTransMatrixCache.end() && v_cache->second.version == version)
        {
            return v_cache->second.value;
        }

        // accumulate transform matrix in parent chain
        float4x4                transform = e.GetComponent<TransformComponent>().GetTransformMatrix();
        std::shared_ptr<Entity> parent    = e.GetParent();
        while (parent != nullptr)
        {
            transform = transform * parent->GetComponent<TransformComponent>().GetTransformMatrix();
            parent    = parent->GetParent();
        }

        m_AbsTransMatrixCache[eid] = {transform, version};
        return transform;
    }
} // namespace GE