#pragma once

#include "GE_pch.h"

#include "imgui.h"

#include "Runtime/core/Base/Singleton.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/JsonResource.h"

#include "../RenderPipeline.h"

namespace GE
{
    enum class MaterialType
    {
        FORWARD,
        DEFERRED
    };

    class MaterialManager;

#define GE_MATERIAL_COMMON(type) \
public: \
    inline std::string GetType() const override \
    { \
        return #type; \
    } \
    static inline std::string GetTypeStatic() \
    { \
        return #type; \
    }

#define GE_FORWARD_MATERIAL_COMMON(type) \
    GE_MATERIAL_COMMON(type) \
    inline virtual std::string GetMode() const override \
    { \
        return "forward"; \
    } \
    type(int id, fs::path path, const json& data) : ForwardMaterial(id, path) \
    { \
        Deserialize(data); \
    } \
    type(int id, fs::path path) : ForwardMaterial(id, path) \
    { \
        if (m_resource->IsValid()) \
        { \
            auto&& data = m_resource->GetData(); \
            Deserialize(data); \
        } \
    }

#define GE_DEFERRED_MATERIAL_COMMON(type) \
    GE_MATERIAL_COMMON(type) \
    inline virtual std::string GetMode() const override \
    { \
        return "deferred"; \
    } \
    type(int id, fs::path path, const json& data) : DeferredMaterial(id, path) \
    { \
        Deserialize(data); \
    } \
    type(int id, fs::path path) : DeferredMaterial(id, path) \
    { \
        if (m_resource->IsValid()) \
        { \
            auto&& data = m_resource->GetData(); \
            Deserialize(data); \
        } \
    }

    class Material
    {
        friend class MaterialManager;

    public:
        Material(int id, fs::path path) :
            m_id(id), m_resource(ResourceManager::GetResource<JsonResource>(path, JsonIdentifier::MATERIAL))
        {}

        inline void Save()
        {
            auto&& data = Serialize();
            data["type"]  = GetType();
            data["mode"]  = GetMode();
            data["alias"] = m_alias;
            m_resource->SaveData(data);
        }
        inline fs::path GetPath() const { return m_resource->GetFilePath(); }

        inline int         GetID() const { return m_id; }
        inline std::string GetAlias() const { return m_alias.empty() ? "unnamed material" : m_alias; }

        virtual std::string GetType() const  = 0;
        virtual std::string GetMode() const  = 0;
        virtual bool        IsOpaque() const = 0;
        virtual void        Inspect()        = 0;

        virtual void Deserialize(const json& data) = 0;
        virtual json Serialize()                   = 0;

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