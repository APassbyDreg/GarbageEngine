#pragma once

#include "GE_pch.h"

#include "Runtime/function/Scene/Entity.h"

#include "../RenderPass.h"

#include "../Material/Material.h"
#include "../Mesh/Mesh.h"
#include "../VulkanManager/AutoGpuBuffer.h"
#include "../VulkanManager/GpuImage.h"

#include "../RenderResource.h"

#include "../Shared/ViewUniform.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace GE
{
    using MeshMaterialPairKey = std::tuple<Mesh*, Material*>;

    struct ForwardShadingPassData
    {
        VkExtent2D                           viewport_size;
        std::vector<std::shared_ptr<Entity>> renderables;
        std::shared_ptr<Mesh>                m_mesh;
        std::shared_ptr<ForwardMaterial>     m_material;
    };

    class ForwardShadingPass : public GraphicsPass, public std::enable_shared_from_this<ForwardShadingPass>
    {
    public:
        ForwardShadingPass(std::shared_ptr<Mesh>                  mesh,
                           std::shared_ptr<ForwardMaterial>       material,
                           std::vector<std::shared_ptr<GpuImage>> color_targets,
                           std::vector<std::shared_ptr<GpuImage>> depth_targets,
                           RenderResourceManager&                 resource_manager) :
            GraphicsPass(resource_manager,
                         std::format("ForwardShading-{}({})-{}({})",
                                     material->GetType(),
                                     (void*)material.get(),
                                     mesh->GetType(),
                                     (void*)mesh.get())),
            m_colorTargets(color_targets), m_depthTargets(depth_targets)
        {
            m_mesh      = mesh->GetType();
            m_material  = material->GetType();
            m_numFrames = color_targets.size();

            m_pipelineSetupFunc = [=](GraphicsRenderPipeline& pipeline) {
                mesh->SetupRenderPass(shared_from_this());
                material->SetupShadingPipeline(pipeline);
            };
        }
        ~ForwardShadingPass() {};

        void Resize(uint width, uint height) override;

        void Run(RenderPassRunData run_data, ForwardShadingPassData pass_data);

        inline VkSemaphore GetFinishedSemaphore(uint frame_idx)
        {
            return m_resourceManager.GetPerFrameSemaphore(frame_idx, FullIdentifier("Finished"))->Get();
        }

    protected:
        virtual void InitInternal(uint frame_cnt) override;

    private:
        std::string                                  m_mesh, m_material;
        std::function<void(GraphicsRenderPipeline&)> m_pipelineSetupFunc;

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