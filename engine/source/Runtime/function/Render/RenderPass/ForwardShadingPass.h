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

namespace GE
{
    using MeshMaterialPairKey = std::tuple<Mesh*, Material*>;

    struct ForwardShadingPassData
    {
        VkExtent2D                           viewport_size;
        std::vector<std::shared_ptr<Entity>> renderables;
        std::shared_ptr<Mesh>                mesh;
        std::shared_ptr<ForwardMaterial>     material;
    };

    class ForwardShadingPass : public GraphicsPass<ForwardShadingPass>,
                               public std::enable_shared_from_this<ForwardShadingPass>
    {
    public:
        ForwardShadingPass(std::shared_ptr<Mesh>                  mesh,
                           std::shared_ptr<ForwardMaterial>       material,
                           std::vector<std::shared_ptr<GpuImage>> color_targets,
                           std::vector<std::shared_ptr<GpuImage>> depth_targets,
                           RenderResourceManager&                 resource_manager) :
            GraphicsPass(resource_manager,
                         std::format("ForwardShading-[{}]{}-[{}]{}",
                                     material->GetType(),
                                     material->GetID(),
                                     mesh->GetType(),
                                     mesh->GetID())),
            m_colorTargets(color_targets), m_depthTargets(depth_targets)
        {
            m_meshName     = mesh->GetType();
            m_materialName = material->GetType();
            m_numFrames = color_targets.size();
            m_pipelineSetupFns.push_back([=](GraphicsRenderPipeline& pipeline) {
                mesh->SetupRenderPipeline(pipeline);
                material->SetupRenderPipeline(pipeline);
            });
            m_passSetupFns.push_back([=](RenderPassBase& pass) {
                auto& graphic_pass = dynamic_cast<GraphicsPassBase&>(pass);
                mesh->SetupRenderPass(graphic_pass);
                material->SetupRenderPass(graphic_pass);
            });
        }
        ~ForwardShadingPass() {};

        void Init(uint frame_cnt) override;
        void Resize(uint width, uint height) override;

        void Run(RenderPassRunData run_data, ForwardShadingPassData pass_data);

        inline VkSemaphore GetFinishedSemaphore(uint frame_idx)
        {
            return m_resourceManager.GetPerFrameSemaphore(frame_idx, FullIdentifier("Finished"))->Get();
        }

        inline std::string GetPassID() const { return m_meshName + "-" + m_materialName; }

    protected:
    private:
        std::string                               m_meshName, m_materialName;
        std::vector<std::shared_ptr<GpuImage>>    m_colorTargets, m_depthTargets;
        std::vector<std::shared_ptr<FrameBuffer>> m_frameBuffers;
        uint                                      m_numFrames = 0;
    };

    struct CombinedForwardShadingPassData
    {
        std::map<MeshMaterialPairKey, std::vector<std::shared_ptr<Entity>>> renderables;
    };

    class CombinedForwardShadingPass
    {
    public:
        CombinedForwardShadingPass(RenderResourceManager& resource_manager) : m_resourceManager(resource_manager) {}

        inline void Init(uint num_frames) { m_framesParallel = num_frames; }

        void Resize(uint width, uint height);

        void Run(RenderPassRunData run_data, CombinedForwardShadingPassData pass_data);

    private:
        std::map<MeshMaterialPairKey, std::shared_ptr<ForwardShadingPass>> m_passes;
        std::map<MeshMaterialPairKey, std::shared_ptr<Semaphore>>          m_semaphores;
        RenderResourceManager&                                             m_resourceManager;

        VkExtent2D m_size           = {0, 0};
        uint       m_framesParallel = 0;
    };
} // namespace GE