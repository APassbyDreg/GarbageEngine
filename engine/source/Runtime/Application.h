#pragma once

#include "GE_pch.h"

#include "core/Events/EventSystem.h"
#include "core/LogSystem.h"
#include "core/Window.h"

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

    private:
        std::unique_ptr<Window> m_Window;
        bool                    m_Running = true;
    };

    // Defined in client
    Application* CreateApplication();
} // namespace GE
