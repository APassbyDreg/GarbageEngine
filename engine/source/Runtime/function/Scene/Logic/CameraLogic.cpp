#include "CameraLogic.h"

namespace GE
{
    float4x4 CameraLogic::GetVMatrix(const Entity& e)
    {
        uint         version = e.GetVersion();
        int eid     = e.GetEntityID();
        // check cache
        auto v_cache = m_VMatrixCache.find(eid);
        if (v_cache == m_VMatrixCache.end() && v_cache->second.version == version)
        {
            return v_cache->second.value;
        }
        // compute new one
        const TransformComponent& trans       = e.GetComponent<TransformComponent>();
        float4x4                  view_matrix = m_defaultLookAt * glm::inverse(trans.GetTransformMatrix());
        m_VMatrixCache[eid]                   = {view_matrix, version};
        return view_matrix;
    }

    float4x4 CameraLogic::GetPMatrix(const Entity& e)
    {
        uint version = e.GetVersion();
        int  eid     = e.GetEntityID();
        // check cache
        auto p_cache = m_PMatrixCache.find(eid);
        if (p_cache == m_PMatrixCache.end() && p_cache->second.version == version)
        {
            return p_cache->second.value;
        }
        // compute new one
        const CameraComponent& cam         = e.GetComponent<CameraComponent>();
        float4x4               proj_matrix = cam.GetProjectionMatrix();
        m_PMatrixCache[eid]                = {proj_matrix, version};
        return proj_matrix;
    }

    float4x4 CameraLogic::GetVPMatrix(const Entity& e)
    {
        uint version = e.GetVersion();
        int  eid     = e.GetEntityID();
        // check cache
        auto vp_cache = m_VPMatrixCache.find(eid);
        if (vp_cache == m_VPMatrixCache.end() && vp_cache->second.version == version)
        {
            return vp_cache->second.value;
        }
        // compute new one
        float4x4 v_matrix    = GetVMatrix(e);
        float4x4 p_matrix    = GetPMatrix(e);
        float4x4 vp_matrix   = p_matrix * v_matrix;
        m_VPMatrixCache[eid] = {vp_matrix, version};
        return vp_matrix;
    }
} // namespace GE