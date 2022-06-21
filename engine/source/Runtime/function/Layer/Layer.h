#pragma once

#include "GE_pch.h"

#include "Runtime/function/Event/EventSystem.h"

namespace GE
{
    class GE_API Layer
    {
    public:
        Layer(const std::string name = "Layer") { m_layerName = name; }
        virtual ~Layer() {};

        virtual void OnEnable() {}
        virtual void OnDisable() {}
        virtual void OnAttach() {}
        virtual void OnDetatch() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event& event) {}

        inline const std::string& GetName() const { return m_layerName; }

    private:
        std::string m_layerName;
        bool        m_isEnabled = true;
    };

} // namespace GE