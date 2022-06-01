#pragma once

#include "GE_pch.h"

#include "function/Event/EventSystem.h"
#include "function/Layer/LayerSystem.h"
#include "function/Log/LogSystem.h"
#include "function/Message/MessageSystem.h"
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

        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);

    private:
        std::shared_ptr<Window> m_window;
        bool                    m_running = true;
        LayerStack              m_layerStack;

        std::shared_ptr<MsgResultBase> __handle_window_close(WindowCloseMsg msg);
    };

    // Defined in client
    std::unique_ptr<Application> CreateApplication();
} // namespace GE
