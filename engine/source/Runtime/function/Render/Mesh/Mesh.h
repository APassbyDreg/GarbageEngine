#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Bounds.h"
#include "Runtime/core/Math/Math.h"

#include "../RenderPipeline.h"
#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/VulkanCore.h"

#include "../RenderPass.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/JsonResource.h"

#include "Runtime/function/Scene/Entity.h"

namespace GE
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct MeshRenderPassData
    {
        uint                                 frame_idx;
        VkCommandBuffer                      cmd;
        VkPipelineLayout                     layout;
        std::vector<std::shared_ptr<Entity>> renderables;
        std::shared_ptr<GraphicsPass>        resource_manager;
    };

#define GE_MESH_COMMON(type) \
public: \
    inline std::string GetType() const override \
    { \
        return #type; \
    } \
    static inline std::string GetTypeStatic() \
    { \
        return #type; \
    } \
    type(int id, fs::path path, const json& data) : Mesh(id, path) \
    { \
        Deserialize(data); \
    } \
    type(int id, fs::path path) : Mesh(id, path) \
    { \
        if (m_resource->IsValid()) \
        { \
            auto&& data = m_resource->GetData(); \
            Deserialize(data); \
        } \
    }

    class Mesh
    {
    public:
        Mesh(int id, fs::path path) :
            m_id(id), m_resource(ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MESH))
        {}

        virtual void SetupRenderPass(std::shared_ptr<GraphicsPass> pass) = 0;
        virtual void RunRenderPass(MeshRenderPassData data)              = 0;

        inline int         GetID() const { return m_id; }
        inline std::string GetAlias() const { return m_alias.empty() ? "unnamed mesh" : m_alias; }

        virtual std::string GetType() const = 0;
        virtual void        Inspect()       = 0;

        virtual Bounds3f& BBox() = 0;

        virtual void Deserialize(const json& data) = 0;
        virtual json Serialize()                   = 0;

        inline void Save()
        {
            auto&& data  = Serialize();
            data["type"] = GetType();
            m_resource->SaveData(data);
        }
        inline fs::path GetPath() const { return m_resource->GetFilePath(); }

    protected:
        int                           m_id    = -1;
        std::string                   m_alias = "mesh";
        std::shared_ptr<JsonResource> m_resource;
    };
} // namespace GE