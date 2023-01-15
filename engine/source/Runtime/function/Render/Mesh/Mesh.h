#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Bounds.h"
#include "Runtime/core/Math/Math.h"

#include "../RenderPipeline.h"
#include "../VulkanManager/VulkanCore.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/JsonResource.h"

#include "imgui.h"

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
        VkCommandBuffer  cmd;
        VkPipelineLayout layout;
        uint num_instance;
    };

#define GE_MESH_COMMON(name) \
public: \
    inline std::string GetName() const override \
    { \
        return #name; \
    } \
    static inline std::string GetNameStatic() \
    { \
        return #name; \
    } \
    name(int id, fs::path path, const json& data) : Mesh(id, path) \
    { \
        Deserialize(data); \
    } \
    name(int id, fs::path path) : Mesh(id, path) \
    {}

    class Mesh
    {
    public:
        Mesh(int id, fs::path path) :
            m_id(id), m_resource(ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MESH))
        {
            if (m_resource->IsValid())
            {
                auto&& data = m_resource->GetData();
                Deserialize(data);
            }
        }

        virtual void SetupPipeline(GraphicsRenderPipeline& pipeline) = 0;
        virtual void RunRenderPass(MeshRenderPassData data)          = 0;

        inline int         GetID() const { return m_id; }
        inline std::string GetAlias() const { return m_alias; }

        virtual std::string GetName() const = 0;
        virtual void        Inspect()       = 0;

        virtual Bounds3f& BBox() = 0;

        virtual void Deserialize(const json& data) {}
        virtual json Serialize() { return {}; }

        inline void Save()
        {
            auto&& data  = Serialize();
            data["type"] = GetName();
            m_resource->SaveData(data);
        }

    protected:
        int                           m_id    = -1;
        std::string                   m_alias = "material";
        std::shared_ptr<JsonResource> m_resource;
    };
} // namespace GE