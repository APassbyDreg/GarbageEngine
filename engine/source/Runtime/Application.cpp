#include "GE_pch.h"

#include "Application.h"

#include "GLFW/glfw3.h"

#define GE_BIND_CLASS_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GE
{
    Application::Application()
    {
        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(GE_BIND_CLASS_FN(Application::OnEvent));
    }

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Starting Application");

        while (m_running)
        {
            for (auto&& layer : m_layerStack)
            {
                layer->OnTick();
            }

            m_window->OnTick();
        }

        GE_CORE_TRACE("Ending Application");
    }

    void Application::OnEvent(Event& e)
    {
        // GE_CORE_TRACE(e.toString());
        auto it   = m_layerStack.end();
        bool done = false;
        while (!e.m_handled)
        {
            if (it == m_layerStack.begin())
            {
                break;
            }
            it--;
            (*it)->OnEvent(e);
        }

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(GE_BIND_CLASS_FN(Application::OnWindowClosed));
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
        m_running = false;
        return true;
    }

    void Application::PushLayer(std::shared_ptr<Layer> layer)
    {
        m_layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(std::shared_ptr<Layer> overlay)
    {
        m_layerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }
} // namespace GE
