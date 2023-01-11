#pragma once

#include "../Material.h"

#include "Runtime/core/Math/Math.h"

namespace GE
{
    class ForwardSolidMaterial : public ForwardMaterial
    {
        GE_FORWARD_MATERIAL_COMMON(ForwardSolidMaterial);

    public:
        ForwardSolidMaterial(int id, fs::path path, float4 color) : m_color(color), ForwardMaterial(id, path) {}

        bool IsOpaque() const override;

        void Deserialize(const json& data) override;
        json Serialize() override;
        void Inspect() override;
        
        void SetupShadingPass(MaterialRenderPassData data) override;
        void SetupShadingPipeline(GraphicsRenderPipeline& pipeline) override;

    private:
        float4 m_color;
    };
} // namespace GE