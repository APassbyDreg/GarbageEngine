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

        m_window = Window::Create();
        m_window->SetEventCallback(GE_BIND_CLASS_FN(Application::OnEvent));

        VulkanManager::GetInstance().Init(m_window->GetNativeWindow());
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

            m_window->OnUpdate();
        }

        GE_CORE_TRACE("Ending Application");
    }

    void Application::OnEvent(Event& e)
    {
        auto it   = m_layerStack.end();
        bool done = false;

        // firstly, handle window events
        if (e.getEventType() == EventType::WindowClose)
        {
            m_running = false;
        }

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

    std::shared_ptr<MsgResultBase> Application::__handle_window_close(WindowCloseMsg msg)
    {
        m_running = false;
        return MsgResultBase::Success();
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
