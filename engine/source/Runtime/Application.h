#pragma once

#include "GE_pch.h"

#include "Runtime/function/Event/EventSystem.h"
#include "Runtime/function/Layer/LayerSystem.h"
#include "Runtime/function/Log/LogSystem.h"
#include "Runtime/function/Message/MessageSystem.h"
#include "Runtime/function/Scene/Scene.h"
#include "Runtime/function/Window/Window.h"

namespace GE
{
    class GE_API Application
    {
    public:
        void Run();
        void Shutdown();

        Application(/* args */);
        virtual ~Application();

        void OnEvent(Event& e);

        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);

        void OpenScene(std::string path);

        inline static Application&     GetInstance() { return *s_instance; }
        inline std::shared_ptr<Window> GetActiveWindow() { return m_activeWindow; }
        inline std::shared_ptr<Scene>  GetActiveScene() { return m_activeScene; }
        inline std::string             GetWorkDirectory() { return m_workDir; }
        inline void                    SetWorkDirectory(const std::string& workDir) { m_workDir = workDir; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

    private:
        bool       m_running = true;
        LayerStack m_layerStack;

        static Application* s_instance;

        std::shared_ptr<Window> m_activeWindow;

        std::shared_ptr<Scene> m_activeScene;

        std::string m_workDir;
    };

    // Defined in client
    std::unique_ptr<Application> CreateApplication();
} // namespace GE
