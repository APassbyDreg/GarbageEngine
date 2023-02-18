#pragma once

#include "GE_pch.h"

#include "../Entity.h"

namespace GE
{
    struct CameraInfo
    {
        int      eid = -2; // -1 for default camera & -2 for uninitialized object
        float4x4 v_matrix {1.0}, p_matrix {1.0};
        float3   position = {0.0, 0.0, 0.0};
        float2   clip     = {0.1f, 1000.0f};
    };

    class SceneCameraManager
    {
        friend class Scene;
        using CacheKey = std::tuple<int, int, float>;

        // Camera looks at z direction with y as up in local coord
        const float4x4 c_defaultLookAt = glm::lookAt(float3(0, 0, 0), float3(0, 0, 1), float3(0, 1, 0));

    public:
        SceneCameraManager(Scene& sc) : m_scene(sc) {}
        void Setup();
        void Destroy();

        CameraInfo GetActiveCameraInfo(float aspect);
        CameraInfo GetCameraInfo(std::shared_ptr<Entity> e, float aspect);

    private:
        float2   GetClip(const Entity& e);
        float4x4 GetVMatrix(const Entity& e);
        float4x4 GetPMatrix(const Entity& e, float aspect);
        float4x4 GetVPMatrix(const Entity& e, float aspect);

        void UpdateEntity(Entity& entity);

    private:
        std::map<CacheKey, float4x4> m_VMatrixCache;
        std::map<CacheKey, float4x4> m_PMatrixCache;
        std::map<CacheKey, float4x4> m_VPMatrixCache;
        std::map<CacheKey, float2>   m_ClipCache;

        std::shared_ptr<Entity> m_activeCamera = nullptr;
        Scene&                  m_scene;
    };
} // namespace GE