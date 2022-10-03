#pragma once

#include "GE_pch.h"

#include "ComponentBase.h"

namespace GE
{
    class RendererComponent : public ComponentBase
    {
    public:
        GE_COMPONENT_COMMON(RendererComponent);

        inline json Serialize() const override { return {}; }
        inline void Deserialize(const json& data) override {}
        inline void Inspect() override {}

    private:
    };

} // namespace GE