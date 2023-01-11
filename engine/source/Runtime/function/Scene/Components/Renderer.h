#pragma once

#include "GE_pch.h"

#include "ComponentBase.h"

namespace GE
{
    struct RendererComponentCore
    {
        uint material_id = 0;
    };

    class RendererComponent : public ComponentBase
    {
        GE_COMPONENT_COMMON(RendererComponent);

    public:
        inline json Serialize() const override
        {
            auto [material_id] = m_core.GetValue();
            return {"material_id", material_id};
        }
        inline void Deserialize(const json& data) override
        {
            uint material_id = data["material_id"].get<uint>();
            m_core           = {material_id};
        }
        inline void Inspect() override
        {
            auto [material_id] = m_core.GetValue();

            ImGui::DragInt("Material ID", (int*)&material_id);

            auto [old_material_id] = m_core.GetValue();
            if (old_material_id != material_id)
            {
                m_core = {material_id};
            }
        }

    private:
    };

} // namespace GE