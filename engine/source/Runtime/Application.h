#pragma once

#include "GE_pch.h"

#include "core/Layer/LayerSystem.h"

#include "function/Event/EventSystem.h"
#include "function/Log/LogSystem.h"
#include "function/UI/Window.h"

namespace GE
{
    class GE_API Application
    {
    public:
        void Run();

        Application(/* args */);
        virtual ~Application();

        void OnEvent(Event& e);

        bool OnWindowClosed(WindowCloseEvent& e);

        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);

    private:
        std::unique_ptr<Window> m_window;
        bool                    m_running = true;
        LayerStack              m_layerStack;
    };

    // Defined in client
    std::unique_ptr<Application> CreateApplication();
} // namespace GE
