#pragma once

#include "GE_pch.h"

#include "Runtime/function/Scene/Entity.h"

#include "../RenderPass.h"

#include "../Material/Material.h"
#include "../Mesh/Mesh.h"
#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/GpuImage.h"

#include "../RenderResource.h"

#include "../Manager/PerSceneDataManager.h"
#include "../Shared/ViewUniform.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>

namespace GE
{
    using MeshMaterialInstancePairKey = std::tuple<Mesh*, Material*>;
    using MeshMaterialTypePairKey     = std::tuple<std::string, std::string>;

    struct OpaqueForwardShadingPassData
    {
        std::map<MeshMaterialInstancePairKey, std::vector<std::shared_ptr<Entity>>> renderables;
    };

    class OpaqueForwardShadingPassUnit : public GraphicsPassUnit
    {
    public:
        OpaqueForwardShadingPassUnit(GraphicsPass&                    pass,
                                     std::shared_ptr<Mesh>            mesh,
                                     std::shared_ptr<ForwardMaterial> material);

        void Run(uint frame_id, VkCommandBuffer cmd, std::vector<std::shared_ptr<Entity>> instances);

        inline std::string FullIdentifier(std::string identifier) override
        {
            auto mesh     = m_mesh.lock();
            auto material = m_material.lock();
            return m_pass.FullIdentifier(
                std::format("{}({})-{}({})/", mesh->GetType(), mesh->GetID(), material->GetType(), material->GetID()));
        }

    private:
        std::weak_ptr<ForwardMaterial> m_material;
        std::weak_ptr<Mesh>            m_mesh;
    };

    class OpaqueForwardShadingPass : public GraphicsPass
    {
    public:
        OpaqueForwardShadingPass(RenderResourceManager& resource_manager) :
            GraphicsPass(resource_manager, "ForwardShading")
        {}

        void Init(uint frame_cnt) override;
        void Resize(uint width, uint height) override;

        void Run(RenderPassRunData run_data, OpaqueForwardShadingPassData pass_data);

    private:
        std::map<MeshMaterialInstancePairKey, std::shared_ptr<OpaqueForwardShadingPassUnit>> m_units;
    };
} // namespace GE