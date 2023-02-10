#pragma once

#include "ComponentBase.h"

#include "Runtime/function/Render/Mesh/Mesh.h"

namespace GE
{
    typedef std::shared_ptr<Mesh> MeshComponentCore;

    class MeshComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(MeshComponent);

    public:
        json Serialize() const override;

        void Deserialize(const json& data) override;

        void Inspect() override;

    private:
        void HandleOpenFile();
    };
} // namespace GE