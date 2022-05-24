#pragma once

#include "GE_pch.h"

#include "core/Window.h"

#include "GLFW/glfw3.h"

namespace GE
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProperties& props);
        virtual ~WindowsWindow();

        virtual void OnTick() override;

        virtual uint GetWidth() const override { return m_Data.width; }
        virtual uint GetHeight() const override { return m_Data.height; }

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }

    private:
        virtual void Init(const WindowProperties& props);
        virtual void Shutdown();

    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string  title;
            unsigned int width, height;

            EventCallbackFn eventCallback;
        };

        WindowData m_Data;

        static bool s_GLFWInitialized;
    };
} // namespace GE
