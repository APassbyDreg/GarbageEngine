#include "GE_pch.h"

#include "Application.h"
#include "Runtime/function/Message/MessageSystem.h"
#include "Runtime/function/Render/VulkanManager/VulkanManager.h"

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
        WindowProperties        main_window_props("GE Engine", 1920, 1080, GE_BIND_CLASS_FN(Application::OnEvent));
        std::shared_ptr<Window> main_window_layer = std::make_shared<Window>(main_window_props);
        m_activeWindow                            = main_window_layer;

        // init subsystems
        VulkanManager::GetInstance().Init(main_window_layer->GetNativeWindow());
    }

    Application::~Application() {}

    void Application::Run()
    {
        GE_CORE_TRACE("Starting Application");

        float  fps = -1.0f;
        double t0, t1, last_update = 0.0;
        while (m_running)
        {
            /* ------------------------- begin frame ------------------------ */
            t0 = glfwGetTime();

            /* ------------------------ update layers ----------------------- */
            for (auto&& layer : m_layerStack)
            {
                layer->OnUpdate();
            }

            /* ------------------------ do rendering ------------------------ */

            /* ---------------------- show imgui window --------------------- */
            m_activeWindow->BeginWindowRender();

            // show fps
            if (t0 - last_update > 0.5)
            {
                last_update = t0;
                m_activeWindow->SetTitle("fps: " + std::to_string(fps));
            }
            // debug
            bool _show_demo_window = true;
            ImGui::ShowDemoWindow(&_show_demo_window);
            // show layers
            ImGuiContext* ctx = m_activeWindow->GetImGuiContext();
            for (auto&& layer : m_layerStack)
            {
                layer->OnImGuiRender(ctx);
            }

            m_activeWindow->EndWindowRender();

            /* ----------------------- finalize frame ----------------------- */
            t1  = glfwGetTime();
            fps = fps * 0.9 + 0.1 * 1.0f / (t1 - t0);
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
