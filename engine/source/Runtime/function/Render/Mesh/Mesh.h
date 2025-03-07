#pragma once

#include "GE_pch.h"

#include "Runtime/core/Base/Serializable.h"
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
    struct MeshRenderPassData
    {
        uint                                 frame_idx;
        VkCommandBuffer                      cmd;
        std::vector<std::shared_ptr<Entity>> renderables;
        GraphicsPassUnit&                    unit;
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

    class Mesh : public Serializable<json>
    {
    public:
        const uint c_meshDataDescriptorID     = 2;
        const uint c_instanceDataDescriptorID = 4;
        const uint c_meshInstanceDataBinding  = 0;

    public:
        Mesh(int id, fs::path path) :
            m_id(id), m_resource(ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MESH))
        {
            GE_CORE_TRACE("Created Mesh: {}", path.string());
        }
        virtual ~Mesh() { GE_CORE_TRACE("Destroyed Mesh: {}", GetPath().string()); }

        virtual void SetupRenderPipeline(GraphicsRenderPipeline& pipeline) = 0;
        virtual void SetupPassResources(GraphicsPassUnit& unit)            = 0;
        virtual void RunRenderPass(MeshRenderPassData data)                = 0;

        inline int         GetID() const { return m_id; }
        inline std::string GetAlias() const { return m_alias.empty() ? "unnamed mesh" : m_alias; }

        virtual std::string GetType() const = 0;
        virtual void        Inspect()       = 0;

        virtual Bounds3f& BBox() = 0;

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