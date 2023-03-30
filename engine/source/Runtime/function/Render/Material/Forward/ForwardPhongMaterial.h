#pragma once

#include "../Material.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    class ForwardPhongMaterial : public ForwardMaterial
    {
        GE_FORWARD_MATERIAL_COMMON(ForwardPhongMaterial);

    public:
        inline bool IsOpaque() const override { return true; }

        void Deserialize(const json& data) override;
        json Serialize() const override;
        void Inspect() override;

        void RunShadingPass(MaterialRenderPassData data) override;
        void SetupRenderPipeline(GraphicsRenderPipeline& pipeline) override;
        void SetupPassResources(GraphicsPassUnit& pass) override;

    private:
        PhongParams m_params;
    };
} // namespace GE