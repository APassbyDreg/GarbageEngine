#include "MeshManager.h"

#include "../Hooks.h"
#include "../Scene.h"

#include "../Components/InstancedMesh.h"
#include "../Components/Transform.h"

#define BIND_CLASS_FN(fn) std::bind(&MeshManager::fn, this, std::placeholders::_1)

namespace GE
{
    inline bool IsManagable(Entity& e)
    {
        return e.HasComponent<InstancedMeshComponent>() && e.HasComponent<TransformComponent>();
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
            auto& nodes = eid2node[eid];
            for (auto it = nodes.begin(); it != nodes.end(); it++)
            {
                if ((*it) == shared_from_this())
                {
                    nodes.erase(it);
                    break;
                }
            }
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
        // spawn child if needed
        if (children.empty() && elements.size() > c_splitThres)
        {
            SpawnChild();
        }

        // create eid2node entry if needed
        if (eid2node.find(e->GetEntityID()) == eid2node.end())
        {
            eid2node[e->GetEntityID()] = std::vector<std::shared_ptr<SceneOctreeNode>>();
        }

        if (children.empty())
        {
            elements.push_back(e);
            eid2node[e->GetEntityID()].push_back(shared_from_this());
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
                eid2node[e->GetEntityID()].push_back(shared_from_this());
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
                auto node_list = eid2node[e->GetEntityID()];
                for (int i = 0; i < node_list.size(); i++)
                {
                    if (node_list[i] == child)
                    {
                        node_list[i] = shared_from_this();
                        break;
                    }
                }
                elements.push_back(e);
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
                    auto node_list = eid2node[elem->GetEntityID()];
                    for (int i = 0; i < node_list.size(); i++)
                    {
                        if (node_list[i] == thisp)
                        {
                            node_list[i] = child;
                            break;
                        }
                    }
                    child->elements.push_back(elem);
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

    void MeshManager::Setup()
    {
        auto sc_name = m_scene.GetName();
        ComponentHook<InstancedMeshComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), sc_name);
        ComponentHook<InstancedMeshComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), sc_name);
        ComponentHook<InstancedMeshComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
        ComponentHook<TransformComponent>::AddConstructHook(BIND_CLASS_FN(AddEntity), sc_name);
        ComponentHook<TransformComponent>::AddDestructHook(BIND_CLASS_FN(RemoveEntity), sc_name);
        ComponentHook<TransformComponent>::AddChangedHook(BIND_CLASS_FN(UpdateEntity), sc_name);
    }

    void MeshManager::AddEntity(Entity& e)
    {
        if (!IsManagable(e))
        {
            return;
        }

        Bounds3f aabb = EntityAABBLogic::GetInstance().GetAABB(e);

        int3 min_ids = glm::floor(aabb.Min() / c_baseResolution + 0.5f);
        int3 max_ids = glm::floor(aabb.Max() / c_baseResolution + 0.5f);
        auto pe      = e.AsPtr();
        for (int ix = min_ids.x; ix <= max_ids.x; ix++)
        {
            for (int iy = min_ids.y; iy <= max_ids.y; iy++)
            {
                for (int iz = min_ids.z; iz <= max_ids.z; iz++)
                {
                    TupledInt3 loc    = {ix, iy, iz};
                    float3     center = float3(ix, iy, iz) * c_baseResolution;
                    if (m_root.find(loc) == m_root.end())
                    {
                        m_root[loc] = std::make_shared<SceneOctreeNode>(0, center, c_baseResolution, m_entityToNode);
                    }
                    m_root[loc]->AddElement(pe);
                }
            }
        }
    }
    void MeshManager::RemoveEntity(Entity& e)
    {
        auto eid = e.GetEntityID();
        for (auto&& node : m_entityToNode[eid])
        {
            node->RemoveElement(eid);
        }
    }

    void MeshManager::UpdateEntity(Entity& e)
    {
        if (IsManagable(e) && m_entityToNode.find(e.GetEntityID()) != m_entityToNode.end())
        {
            RemoveEntity(e);
            AddEntity(e);
        }
    }

    void
    FrustrumCullNode(float4x4& vp, std::shared_ptr<SceneOctreeNode> node, std::vector<std::shared_ptr<Entity>>& results)
    {
        auto is = FrustrumAABBIntersection(vp, node->GetAABB());
        if (is == AABBFrustrumIntersection::INSIDE)
        {
            results.insert(results.end(), node->elements.begin(), node->elements.end());
        }
        else if (is == AABBFrustrumIntersection::INTERSECT)
        {
            for (auto&& child : node->children)
            {
                FrustrumCullNode(vp, child, results);
            }
        }
    }

    std::set<std::shared_ptr<Entity>> MeshManager::FrustrumCull(float4x4& vp)
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
        std::set<std::shared_ptr<Entity>> final_results = {};
        for (auto&& [idx, result] : results)
        {
            final_results.insert(result.begin(), result.end());
        }
        return final_results;
    }
} // namespace GE