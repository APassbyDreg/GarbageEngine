#pragma once

#include "GE_pch.h"

#include "CameraManager.h"

#include "../Scene.h"

#include "../Components/Camera.h"
#include "../Components/Transform.h"

namespace GE
{
#define BIND_CLASS_FN(fn) std::bind(&SceneCameraManager::fn, this, std::placeholders::_1)

    static bool IsManagable(Entity& e)
    {
        return e.HasComponent<TransformComponent>() && e.HasComponent<CameraComponent>();
    }

    void SceneCameraManager::Setup()
    {
        auto sc_name = m_scene.GetName();
        ComponentHook<TransformComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<CameraComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);

        ComponentHook<TransformComponent>::AddConstructHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<CameraComponent>::AddConstructHook(BIND_CLASS_FN(UpdateEntity), sc_name);

        ComponentHook<TransformComponent>::AddDestructHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<CameraComponent>::AddDestructHook(BIND_CLASS_FN(UpdateEntity), sc_name);
    }

    void SceneCameraManager::UpdateEntity(Entity& entity)
    {
        bool has_active_camera = m_activeCamera != nullptr;

        if (!IsManagable(entity))
        {
            if (has_active_camera && entity.GetEntityID() == m_activeCamera->GetEntityID())
            {
                m_activeCamera = nullptr;
            }
            return;
        }

        if (entity.GetComponent<CameraComponent>().GetCoreValue().active)
        {
            if (!has_active_camera || m_activeCamera->GetEntityID() != entity.GetEntityID())
            {
                if (has_active_camera)
                {
                    auto&& cam = m_activeCamera->GetComponent<CameraComponent>();
                    auto   val = cam.GetCoreValue();
                    val.active = false;
                    cam.m_core.SetValue(val);
                }
                m_activeCamera = entity.shared_from_this();
            }
        }
        else if (has_active_camera && m_activeCamera->GetEntityID() == entity.GetEntityID())
        {
            m_activeCamera = nullptr;
        }
    }

    CameraInfo SceneCameraManager::GetCameraInfo(std::shared_ptr<Entity> e, float aspect)
    {
        GE_CORE_ASSERT(e != nullptr, "[SceneCameraManager::GetCameraInfo] Called with null camera entity");

        CameraInfo info  = {};
        auto&&     trans = e->GetComponent<TransformComponent>();
        info.eid         = e->GetEntityID();
        info.position    = trans.GetPosition();
        info.clip        = GetClip(*e);
        info.v_matrix    = GetVMatrix(*e);
        info.p_matrix    = GetPMatrix(*e, aspect);
        return info;
    }

    CameraInfo SceneCameraManager::GetActiveCameraInfo(float aspect)
    {
        if (m_activeCamera != nullptr)
        {
            return GetCameraInfo(m_activeCamera, aspect);
        }

        // default case
        CameraInfo info = {};
        info.eid        = -1;
        info.v_matrix   = float4x4(1.0);
        info.p_matrix   = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
        return info;
    }

    float2 SceneCameraManager::GetClip(const Entity& e)
    {
        uint     version = e.GetVersion();
        int      eid     = e.GetEntityID();
        CacheKey key     = {eid, version, 0.0};
        // check cache
        auto v_cache = m_ClipCache.find(key);
        if (v_cache != m_ClipCache.end())
        {
            return v_cache->second;
        }
        // compute new one
        auto&& cam       = e.GetComponent<CameraComponent>().GetCoreValue();
        m_ClipCache[key] = cam.clip;
        return cam.clip;
    }

    float4x4 SceneCameraManager::GetVMatrix(const Entity& e)
    {
        uint     version = e.GetVersion();
        int      eid     = e.GetEntityID();
        CacheKey key     = {eid, version, 0.0};
        // check cache
        auto v_cache = m_VMatrixCache.find(key);
        if (v_cache != m_VMatrixCache.end())
        {
            return v_cache->second;
        }
        // compute new one
        const TransformComponent& trans       = e.GetComponent<TransformComponent>();
        float4x4                  view_matrix = c_defaultLookAt * glm::inverse(trans.GetTransformMatrix());
        m_VMatrixCache[key]                   = view_matrix;
        return view_matrix;
    }

    float4x4 SceneCameraManager::GetPMatrix(const Entity& e, float aspect)
    {
        uint     version = e.GetVersion();
        int      eid     = e.GetEntityID();
        CacheKey key     = {eid, version, aspect};
        // check cache
        auto p_cache = m_PMatrixCache.find(key);
        if (p_cache != m_PMatrixCache.end())
        {
            return p_cache->second;
        }
        // compute new one
        const CameraComponent& cam         = e.GetComponent<CameraComponent>();
        float4x4               proj_matrix = cam.GetProjectionMatrix(aspect);
        m_PMatrixCache[key]                = proj_matrix;
        return proj_matrix;
    }

    float4x4 SceneCameraManager::GetVPMatrix(const Entity& e, float aspect)
    {
        uint     version = e.GetVersion();
        int      eid     = e.GetEntityID();
        CacheKey key     = {eid, version, aspect};
        // check cache
        auto vp_cache = m_VPMatrixCache.find(key);
        if (vp_cache != m_VPMatrixCache.end())
        {
            return vp_cache->second;
        }
        // compute new one
        float4x4 v_matrix    = GetVMatrix(e);
        float4x4 p_matrix    = GetPMatrix(e, aspect);
        float4x4 vp_matrix   = p_matrix * v_matrix;
        m_VPMatrixCache[key] = vp_matrix;
        return vp_matrix;
    }
} // namespace GE