#pragma once

#include "GE_pch.h"

#include "../Shared/ViewUniform.h"

#include "../RenderResource.h"

namespace GE
{

    class GraphicsRenderPipeline;

    class PerViewDataManager
    {
    public:
        static const uint GE_PerViewDescriptorSetID = 1;

        PerViewDataManager(RenderResourceManager& resource_manager) : m_renderResourceManager(resource_manager) {}

        void Init(uint n_frames);

        void RegisterView(std::string name);
        void UpdateView(uint frame, std::string name, ViewUniform& data);

        static VkDescriptorSetLayout GetPerViewDataLayout();

        static void SetupPipeline(GraphicsRenderPipeline& pipeline);

    private:
        uint m_frameCnt = 0;

        RenderResourceManager& m_renderResourceManager;
    };
} // namespace GE