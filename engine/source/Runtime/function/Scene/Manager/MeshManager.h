#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Bounds.h"

#include "../Entity.h"

#include "../Logic/EntityAABB.h"

namespace GE
{
    class Scene;

    class SceneOctreeNode : public std::enable_shared_from_this<SceneOctreeNode>
    {
    public:
        const size_t c_splitThres = 128;

        SceneOctreeNode(int                                              level,
                        float3                                           center,
                        float3                                           size,
                        std::map<int, std::shared_ptr<SceneOctreeNode>>& eid2node,
                        std::shared_ptr<SceneOctreeNode>                 parent = nullptr) :
            level(level),
            center(center), size(size), parent(parent), eid2node(eid2node)
        {}

        inline bool Fits(Bounds3f& bounds)
        {
            return bounds.minX() >= center.x - size.x / 2 && bounds.maxX() <= center.x + size.x / 2 &&
                   bounds.minY() >= center.y - size.y / 2 && bounds.maxY() <= center.y + size.y / 2 &&
                   bounds.minZ() >= center.z - size.z / 2 && bounds.maxZ() <= center.z + size.z / 2;
        }
        inline Bounds3f GetAABB() { return Bounds3f(center - size * 0.5f, center + size * 0.5f); }

        void RemoveElement(int eid, bool is_leaf = true);
        void AddElement(std::shared_ptr<Entity> e);

    private:
        void MergeChild();
        void SpawnChild();

    public:
        int                                           level    = 0;
        size_t                                        num_elem = 0;
        float3                                        size;
        float3                                        center;
        std::shared_ptr<SceneOctreeNode>              parent = nullptr;
        std::vector<std::weak_ptr<SceneOctreeNode>>   children;
        std::vector<std::shared_ptr<Entity>>          elements;

    private:
        std::map<int, std::shared_ptr<SceneOctreeNode>>& eid2node;
    };

    class SceneMeshManager
    {
        friend class Scene;
        using TupledInt3 = std::tuple<int, int, int>;

    public:
        SceneMeshManager(Scene& sc) : m_scene(sc) {}
        void Setup();
        void Destroy();

        std::vector<std::shared_ptr<Entity>> FrustumCull(float4x4& vp_matrix);

    private:
        void AddEntity(Entity& entity);
        void RemoveEntity(Entity& entity);
        void UpdateEntity(Entity& entity);

        bool Exists(int eid);

        const float3 c_baseResolution = {1000.0f, 1000.0f, 100.0f};

        std::map<int, std::shared_ptr<SceneOctreeNode>>        m_entityToNode;
        std::map<int, std::shared_ptr<Entity>>                 m_largeEntities;
        std::map<TupledInt3, std::shared_ptr<SceneOctreeNode>> m_root;

        Scene& m_scene;
    };
} // namespace GE