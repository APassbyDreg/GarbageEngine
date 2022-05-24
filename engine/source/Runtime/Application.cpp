#include "GE_pch.h"

#include "Application.h"
#include "core/Events/ApplicationEvents.h"

namespace GE
{
    Application::Application() { m_Window = std::unique_ptr<Window>(Window::Create()); }

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Starting Application");

        bool running;
        while (running)
        {
            m_Window->OnTick();
        }
    }
} // namespace GE
