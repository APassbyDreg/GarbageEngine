#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/Base/Singleton.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/JsonResource.h"

#include "../RenderPipeline.h"

namespace GE
{
    class MaterialManager;

#define GE_MATERIAL_COMMON(name) \
public: \
    inline std::string GetName() const override \
    { \
        return #name; \
    } \
    static inline std::string GetNameStatic() \
    { \
        return #name; \
    }

#define GE_FORWARD_MATERIAL_COMMON(name) \
    GE_MATERIAL_COMMON(name) \
    name(int id, fs::path path, const json& data) : ForwardMaterial(id, path) \
    { \
        Deserialize(data); \
    } \
    name(int id, fs::path path) : ForwardMaterial(id, path) \
    {}

#define GE_DEFERRED_MATERIAL_COMMON(name) \
    GE_MATERIAL_COMMON(name) \
    name(int id, fs::path path, const json& data) : DeferredMaterial(id, path) \
    { \
        Deserialize(data); \
    } \
    name(int id, fs::path path) : DeferredMaterial(id, path) \
    {}

    class Material
    {
        friend class MaterialManager;

    public:
        Material(int id, fs::path path) :
            m_id(id), m_resource(ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MATERIAL))
        {
            if (m_resource->IsValid())
            {
                auto&& data = m_resource->GetData();
                Deserialize(data);
            }
        }

        inline void Save()
        {
            auto&& data = Serialize();
            data["type"] = GetName();
            m_resource->SaveData(data);
        }

        inline int         GetID() const { return m_id; }
        inline std::string GetAlias() const { return m_alias; }

        virtual std::string GetName() const  = 0;
        virtual bool        IsOpaque() const = 0;
        virtual void        Inspect()        = 0;

        virtual void Deserialize(const json& data) {}
        virtual json Serialize() { return {}; }

    protected:
        int                           m_id    = -1;
        std::string                   m_alias = "material";
        std::shared_ptr<JsonResource> m_resource;
    };

    struct MaterialRenderPassData
    {
        VkCommandBuffer  cmd;
        VkPipelineLayout layout;
    };

    class DeferredMaterial : public Material
    {
    public:
        DeferredMaterial(int id, fs::path path) : Material(id, path) {}

        virtual void SetupGBufferPass(MaterialRenderPassData data)          = 0;
        virtual void SetupShadingPass(MaterialRenderPassData data)          = 0;
        virtual void SetupGBufferPipeline(GraphicsRenderPipeline& pipeline) = 0;
        virtual void SetupShadingPipeline(GraphicsRenderPipeline& pipeline) = 0;
    };

    class ForwardMaterial : public Material
    {
    public:
        ForwardMaterial(int id, fs::path path) : Material(id, path) {}

        virtual void SetupShadingPass(MaterialRenderPassData data)          = 0;
        virtual void SetupShadingPipeline(GraphicsRenderPipeline& pipeline) = 0;
    };
} // namespace GE