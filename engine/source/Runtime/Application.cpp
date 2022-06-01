#include "GE_pch.h"

#include "Application.h"
#include "function/Message/MessageSystem.h"
#include "function/Render/VulkanManager/VulkanManager.h"

#include "GLFW/glfw3.h"

#define GE_BIND_CLASS_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GE
{
    Application::Application()
    {
        m_window = Window::Create();
        m_window->SetEventCallback(GE_BIND_CLASS_FN(Application::OnEvent));

        VulkanManager::GetInstance().Init(m_window->GetNativeWindow());

        MessageDispatcher<WindowCloseMsg, MsgResultBase>::GetInstance().RegisterListener(
            GE_BIND_CLASS_FN(Application::__handle_window_close), 0);
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
