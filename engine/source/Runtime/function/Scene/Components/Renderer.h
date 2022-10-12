#pragma once

#include "GE_pch.h"

#include "ComponentBase.h"

namespace GE
{
    typedef int RendererComponentCore;

    class RendererComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(RendererComponent);

    public:
        inline json Serialize() const override { return {}; }
        inline void Deserialize(const json& data) override {}
        inline void Inspect() override {}

    private:
    };

} // namespace GE