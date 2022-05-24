#include "GE_pch.h"

#include "Application.h"
#include "core/Events/ApplicationEvents.h"

namespace GE
{
    Application::Application() {}

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Application::Run()");

        WindowResizeEvent e(1280, 720);
        GE_CORE_INFO(e.toString());
    }
} // namespace GE
