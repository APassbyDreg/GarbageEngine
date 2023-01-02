#include "EntityAABB.h"

#include "../Components/InstancedMesh.h"
#include "../Components/Transform.h"
#include "../Entity.h"

#include "Transform.h"

namespace GE
{
    Bounds3f EntityAABBLogic::GetAABB(Entity& e)
    {
        uint version = e.GetVersion();
        int  eid     = e.GetEntityID();
        // check cache
        auto cache = m_AABBCache.find(eid);
        if (cache != m_AABBCache.end() && cache->second.version == version)
        {
            return cache->second.value;
        }
        // compute new one
        Bounds3f aabb = float3(0, 0, 0);
        if (e.HasComponent<InstancedMeshComponent>())
        {
            auto&& mesh = e.GetComponent<InstancedMeshComponent>();
            aabb        = mesh.GetCoreValues()->BBox();
        }
        float4x4 transform = TransformLogic::GetInstance().GetAbsoluteTransformMatrix(e);
        return Transform(aabb, transform);
    }
} // namespace GE