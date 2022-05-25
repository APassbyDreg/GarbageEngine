#include "GE_pch.h"

#include "Application.h"

#include "GLFW/glfw3.h"

#define GE_BIND_CLASS_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GE
{
    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(GE_BIND_CLASS_FN(Application::OnEvent));
    }

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Starting Application");

        while (m_Running)
        {
            m_Window->OnTick();
        }
    }

    void Application::OnEvent(Event& e)
    {
        GE_CORE_TRACE(e.toString());

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(GE_BIND_CLASS_FN(Application::OnWindowClosed));
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
} // namespace GE
