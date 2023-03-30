#pragma once

#include "../Material.h"

#include "Runtime/core/Math/Math.h"

#include "Runtime/resource/Managers/ResourceManager.h"
#include "Runtime/resource/ResourceTypes/ImageResource.h"

#include "../../VulkanManager/GpuImage.h"
#include "../../VulkanManager/Sampler.h"

namespace GE
{
    class ForwardTexturedFlatMaterial : public ForwardMaterial
    {
        GE_FORWARD_MATERIAL_COMMON(ForwardTexturedFlatMaterial);

    public:
        ForwardTexturedFlatMaterial(int id, fs::path path, float4 color) : ForwardMaterial(id, path) {}

        bool IsOpaque() const override;

        void Deserialize(const json& data) override;
        json Serialize() const override;
        void Inspect() override;

        void RunShadingPass(MaterialRenderPassData data) override;
        void SetupRenderPipeline(GraphicsRenderPipeline& pipeline) override;
        void SetupPassResources(GraphicsPassUnit& pass) override;

        // load resources into GPU and bind descriptor
        void Activate(VkDescriptorSet descriptor);
        // unload resources
        void Deactivate();

    private:
        Sampler                           m_sampler;
        std::shared_ptr<GpuImage>         m_texture         = std::make_shared<GpuImage>();
        std::shared_ptr<LDRImageResource> m_textureResource = nullptr;
    };
} // namespace GE