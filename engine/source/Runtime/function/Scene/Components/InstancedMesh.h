#pragma once

#include "ComponentBase.h"

#include "Runtime/resource/ResourceTypes/MeshResource.h"

namespace GE
{
    typedef std::shared_ptr<MeshResource> InstancedMeshComponentCore;

    class InstancedMeshComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(InstancedMeshComponent);

    public:
        inline json Serialize() const override { return {}; }

        inline void Deserialize(const json& data) override {}

        void Inspect() override;

    private:
        void HandleOpenFile();
    };
} // namespace GE