#pragma once

#include "GE_pch.h"

#include "Runtime/function/Event/EventSystem.h"

#include "imgui.h"

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
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event& event) {}
        virtual void OnImGuiRender(ImGuiContext* ctx) {}

        inline const std::string& GetName() const { return m_layerName; }
        inline void               Enable() { m_isEnabled = true; }
        inline void               Disable() { m_isEnabled = false; }

    protected:
        friend class Application;
        friend class LayerStack;

        inline void RenderImGui(ImGuiContext* ctx)
        {
            if (m_isEnabled)
            {
                OnImGuiRender(ctx);
            }
        }
        inline void Update()
        {
            if (m_lastEnableStatus != m_isEnabled)
            {
                if (m_isEnabled)
                {
                    OnEnable();
                }
                else
                {
                    OnDisable();
                }
                m_lastEnableStatus = m_isEnabled;
            }

            if (m_isEnabled)
            {
                OnUpdate();
            }
        }

    private:
        std::string m_layerName;
        bool        m_isEnabled        = true;
        bool        m_lastEnableStatus = true;
    };

} // namespace GE