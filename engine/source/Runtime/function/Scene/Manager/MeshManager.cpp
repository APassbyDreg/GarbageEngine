#include "MeshManager.h"

#include "../Hooks.h"
#include "../Scene.h"

#include "../Components/Material.h"
#include "../Components/Mesh.h"
#include "../Components/Transform.h"

#define BIND_CLASS_FN(fn) std::bind(&SceneMeshManager::fn, this, std::placeholders::_1)

namespace GE
{
    static bool IsManagable(Entity& e)
    {
        return e.HasComponent<TransformComponent>() && e.HasComponent<MaterialComponent>() &&
               e.HasComponent<MeshComponent>();
    }

    void SceneOctreeNode::RemoveElement(int eid, bool is_leaf)
    {
        if (is_leaf)
        {
            for (auto it = elements.begin(); it != elements.end(); it++)
            {
                if ((*it)->GetEntityID() == eid)
                {
                    elements.erase(it);
                    break;
                }
            }
            eid2node.erase(eid);
        }

        if (parent != nullptr)
        {
            parent->RemoveElement(eid, false);
        }
        num_elem--;

        if (num_elem < c_splitThres)
        {
            MergeChild();
        }
    }

    void SceneOctreeNode::AddElement(std::shared_ptr<Entity> e)
    {
        int eid = e->GetEntityID();

        // spawn child if needed
        if (children.empty() && elements.size() > c_splitThres)
        {
            SpawnChild();
        }

        if (children.empty())
        {
            elements.push_back(e);
            eid2node[eid] = shared_from_this();
        }
        else
        {
            Bounds3f aabb = EntityAABBLogic::GetInstance().GetAABB(*e);
            bool     fit  = false;
            for (auto&& child : children)
            {
                if (child->Fits(aabb))
                {
                    child->AddElement(e);
                    fit = true;
                    break;
                }
            }
            if (!fit)
            {
                elements.push_back(e);
                eid2node[e->GetEntityID()] = shared_from_this();
            }
        }

        num_elem++;
    }

    void SceneOctreeNode::MergeChild()
    {
        for (auto&& child : children)
        {
            child->MergeChild();
            for (auto&& e : child->elements)
            {
                elements.push_back(e);
                eid2node[e->GetEntityID()] = shared_from_this();
            }
        }
        children.clear();
    }

    void SceneOctreeNode::SpawnChild()
    {
        const std::vector<float3> directions = {
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1}, {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1}};

        auto   thisp        = shared_from_this();
        float3 half_size    = 0.50f * size;
        float3 quarter_size = 0.25f * size;

        // create children nodes
        children.reserve(8);
        for (size_t i = 0; i < 8; i++)
        {
            float3 center_i = center + directions[i] * quarter_size;
            children[i]     = std::make_shared<SceneOctreeNode>(level + 1, center_i, half_size, eid2node, thisp);
        }

        // reassign elements
        float3                               childSize = 0.5f * size;
        std::vector<std::shared_ptr<Entity>> remaining = {};
        for (auto&& elem : elements)
        {
            auto aabb = EntityAABBLogic::GetInstance().GetAABB(*elem);
            bool fit  = false;
            for (auto&& child : children)
            {
                if (child->Fits(aabb))
                {
                    child->AddElement(elem);
                    fit = true;
                    break;
                }
            }
            if (!fit)
            {
                remaining.push_back(elem);
            }
        }
        elements = remaining;
    }

    void SceneMeshManager::Setup()
    {
        auto sc_name = m_scene.GetName();
        ComponentHook<MeshComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), sc_name);
        ComponentHook<MeshComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), sc_name);
        ComponentHook<MeshComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<TransformComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), sc_name);
        ComponentHook<TransformComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), sc_name);
        ComponentHook<TransformComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<MaterialComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), sc_name);
        ComponentHook<MaterialComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), sc_name);
        ComponentHook<MaterialComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
    }

    void SceneMeshManager::AddEntity(Entity& e)
    {
        int eid = e.GetEntityID();

        if (!IsManagable(e))
        {
            return;
        }

        if (Exists(eid))
        {
            RemoveEntity(e);
        }

        Bounds3f aabb = EntityAABBLogic::GetInstance().GetAABB(e);

        int3 min_ids = glm::floor(aabb.Min() / c_baseResolution + 0.5f);
        int3 max_ids = glm::floor(aabb.Max() / c_baseResolution + 0.5f);
        auto pe      = e.AsPtr();
        if (min_ids == max_ids)
        {
            TupledInt3 loc    = {min_ids.x, min_ids.y, min_ids.z};
            float3     center = float3(min_ids) * c_baseResolution;
            if (m_root.find(loc) == m_root.end())
            {
                m_root[loc] = std::make_shared<SceneOctreeNode>(0, center, c_baseResolution, m_entityToNode);
            }
            m_root[loc]->AddElement(pe);
        }
        else
        {
            m_largeEntities[eid] = pe;
        }
    }

    void SceneMeshManager::RemoveEntity(Entity& e)
    {
        int eid = e.GetEntityID();
        if (m_entityToNode.find(eid) != m_entityToNode.end())
        {
            m_entityToNode[eid]->RemoveElement(eid);
        }
        if (m_largeEntities.find(eid) != m_largeEntities.end())
        {
            m_largeEntities.erase(eid);
        }
    }

    bool SceneMeshManager::Exists(int eid)
    {
        return m_entityToNode.find(eid) != m_entityToNode.end() && m_largeEntities.find(eid) != m_largeEntities.end();
    }

    void SceneMeshManager::UpdateEntity(Entity& e)
    {
        if (IsManagable(e) && Exists(e.GetEntityID()))
        {
            RemoveEntity(e);
            AddEntity(e);
        }
    }

    void
    FrustrumCullNode(float4x4& vp, std::shared_ptr<SceneOctreeNode> node, std::vector<std::shared_ptr<Entity>>& results)
    {
        auto is = FrustrumAABBIntersection(vp, node->GetAABB());
        if (is == BoundsIntersectionState::INSIDE)
        {
            results.insert(results.end(), node->elements.begin(), node->elements.end());
        }
        else if (is == BoundsIntersectionState::INTERSECT)
        {
            if (node->children.empty())
            {
                results.insert(results.end(), node->elements.begin(), node->elements.end());
            }
            else
            {
                for (auto&& child : node->children)
                {
                    FrustrumCullNode(vp, child, results);
                }
            }
        }
    }

    std::vector<std::shared_ptr<Entity>> SceneMeshManager::FrustrumCull(float4x4& vp)
    {
        // do fursturm test on each root node
        std::map<TupledInt3, std::vector<std::shared_ptr<Entity>>> results;
        std::vector<std::thread>                                   jobs;
        for (auto&& [idx, node] : m_root)
        {
            results[idx] = std::vector<std::shared_ptr<Entity>>();
            jobs.push_back(std::thread(FrustrumCullNode, std::ref(vp), node, std::ref(results[idx])));
        }
        for (auto&& job : jobs)
        {
            job.join();
        }

        // merge results
        std::vector<std::shared_ptr<Entity>> final_results = {};
        for (auto&& [idx, result] : results)
        {
            std::ranges::move(result, std::back_inserter(final_results));
        }
        for (auto&& [eid, e] : m_largeEntities)
        {
            final_results.push_back(e);
        }
        return final_results;
    }
} // namespace GE