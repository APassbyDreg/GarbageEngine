#pragma once

#include "ComponentBase.h"

namespace GE
{
    class InstancedMeshComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(InstancedMeshComponent);

        std::string m_source = "";

        InstancedMeshComponent() {}

        inline json Serialize() const override { return {}; }

        inline void Deserialize(const json& data) override {}

        inline void Inspect() override
        {
            if (ImGui::CollapsingHeader("InstancedMesh"))
            {}
        }

    private:
    };
} // namespace GE