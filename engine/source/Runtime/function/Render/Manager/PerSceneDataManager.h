#pragma once

#include "GE_pch.h"

#include "../Shared/LightUniform.h"

#include "../RenderResource.h"

namespace GE
{
    class GraphicsRenderPipeline;

    class PerSceneDataManager
    {
    public:
        static const uint GE_PerSceneDescriptorSetID = 0;

        PerSceneDataManager(RenderResourceManager& resource_manager) : m_renderResourceManager(resource_manager) {}

        void Init(uint n_frames);

        void UpdateData(uint frame_id);

        static VkDescriptorSetLayout GetPerSceneDataLayout();

        static void SetupPipeline(GraphicsRenderPipeline& pipeline);

    private:
        std::vector<PointLightData>     m_pointLightData;
        std::vector<SpotLightData>      m_spotLightData;
        std::vector<DirectionLightData> m_directionLightData;

        RenderResourceManager& m_renderResourceManager;
    };
} // namespace GE