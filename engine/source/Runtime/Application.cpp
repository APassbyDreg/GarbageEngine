#include "GE_pch.h"

#include "Application.h"
#include "function/Message/MessageSystem.h"
#include "function/Render/VulkanManager/VulkanManager.h"

#include "GLFW/glfw3.h"

#define GE_BIND_CLASS_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GE
{
    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        GE_CORE_ASSERT(s_instance == nullptr, "Application already exists!");
        s_instance = this;

        // register layers
        WindowProperties             main_window_props("GE Engine", 1920, 1080, GE_BIND_CLASS_FN(Application::OnEvent));
        std::shared_ptr<WindowLayer> main_window_layer = std::make_shared<WindowLayer>(main_window_props);
        m_activeWindowLayer = main_window_layer;
        PushOverlay(main_window_layer);

        // init subsystems
        VulkanManager::GetInstance().Init(main_window_layer->GetNativeWindow());
    }

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Starting Application");

        while (m_running)
        {
            for (auto&& layer : m_layerStack)
            {
                layer->OnUpdate();
            }
        }

        GE_CORE_TRACE("Ending Application");
    }

    void Application::Shutdown() { m_running = false; }

    void Application::OnEvent(Event& e)
    {
        auto it   = m_layerStack.end();
        bool done = false;

        // handle application level events
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(GE_BIND_CLASS_FN(Application::OnWindowClose));

        // propagate event to layers
        while (!e.m_handled)
        {
            if (it == m_layerStack.begin())
            {
                break;
            }
            it--;
            (*it)->OnEvent(e);
        }
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

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_running = false;
        return false;
    }
} // namespace GE
