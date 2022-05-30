#pragma once

#include "GE_pch.h"

#include "function/UI/Window.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace GE
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProperties& props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        uint GetWidth() const override { return m_Data.width; }
        uint GetHeight() const override { return m_Data.height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }

    private:
        void Init(const WindowProperties& props);
        void Shutdown();

    private:
        /* ------------------------- private helpers ------------------------ */

        void init_glfw();
        void init_glfw_callbacks();
        void init_imgui(int2 size);

        void cleanup_glfw();
        void cleanup_imgui();

    private:
        struct WindowData
        {
            std::string  title;
            unsigned int width, height;

            EventCallbackFn eventCallback;
        };

        WindowData m_Data;

        static bool s_glfwInitialized;

        static ImGui_ImplVulkanH_Window s_imguiWindow;
    };
} // namespace GE
