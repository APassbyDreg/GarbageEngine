#pragma once

#include "GE_pch.h"

#include "../RenderPass.h"

#include "../VulkanManager/Sampler.h"

namespace GE
{
    struct ColorMappingPassData
    {
        float gamma;
    };

    class ColorMappingPass : public GraphicsPass
    {
    public:
        ColorMappingPass(RenderResourceManager& resource_manager) : GraphicsPass(resource_manager, "ColorMapping") {}

        void Init(uint frame_cnt) override;
        void Resize(uint width, uint height) override;

        void Run(RenderPassRunData run_data, ColorMappingPassData pass_data);

    private:
        std::shared_ptr<GraphicsRenderPipeline> m_pipeline;

        Sampler m_sampler;
    };
} // namespace GE