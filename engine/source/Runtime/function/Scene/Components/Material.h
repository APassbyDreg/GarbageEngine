#pragma once

#include "ComponentBase.h"

#include "Runtime/function/Render/Material/Material.h"

namespace GE
{
    typedef std::shared_ptr<Material> MaterialComponentCore;

    class MaterialComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(MaterialComponent);

    public:
        json Serialize() const override;

        void Deserialize(const json& data) override;

        void Inspect() override;

    private:
        void HandleOpenFile();
    };
} // namespace GE