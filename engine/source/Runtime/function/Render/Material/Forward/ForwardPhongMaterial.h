#pragma once

#include "../Material.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    struct PhongParams
    {
        alignas(16) float3 kd;
        alignas(16) float3 ks;
        alignas(16) float3 ka;
        float ns;
    };

    class ForwardPhongMaterial : public ForwardMaterial
    {
        GE_FORWARD_MATERIAL_COMMON(ForwardPhongMaterial);

    public:
        inline bool IsOpaque() const override { return true; }

        void Deserialize(const json& data) override;
        json Serialize() override;
        void Inspect() override;

        void RunShadingPass(MaterialRenderPassData data) override;
        void SetupRenderPipeline(GraphicsRenderPipeline& pipeline) override;
        void SetupRenderPass(GraphicsPassBase& pass) override;

    private:
        PhongParams m_params;
    };
} // namespace GE