#pragma once

#include "ComponentBase.h"

#include "Runtime/resource/ResourceTypes/MeshResource.h"

namespace GE
{
    class InstancedMeshComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(InstancedMeshComponent);

        std::shared_ptr<MeshResource> m_source = nullptr;

    public:
        inline json Serialize() const override { return {}; }

        inline void Deserialize(const json& data) override {}

        void Inspect() override;

    private:
        void HandleOpenFile();
    };
} // namespace GE