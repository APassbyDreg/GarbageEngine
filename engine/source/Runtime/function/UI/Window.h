#pragma once

#include "GE_pch.h"

#include "GLFW/glfw3.h"

#include "core/math/math.h"
#include "function/Event/EventSystem.h"

namespace GE
{
    struct WindowProperties
    {
        std::string     title;
        uint            width, height;
        EventCallbackFn eventCallback;

        WindowProperties(const std::string& title         = "GE Engine",
                         uint               width         = 1920,
                         uint               height        = 1080,
                         EventCallbackFn    eventCallback = nullptr) :
            title(title),
            width(width), height(height), eventCallback(eventCallback)
        {}
    };

    /**
     * @brief interface representing a window
     *
     */
    class GE_API Window
    {
    public:
        virtual ~Window() = default;

        inline GLFWwindow* GetNativeWindow() const { return m_glfwWindow; }

        virtual void OnUpdate()        = 0;
        virtual uint GetWidth() const  = 0;
        virtual uint GetHeight() const = 0;
        inline uint2 GetSize() const { return uint2(GetWidth(), GetHeight()); }

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        // To be implemented by platform-specific code
        static std::shared_ptr<Window> Create(const WindowProperties& properties = WindowProperties());

    protected:
        GLFWwindow* m_glfwWindow;
    };
} // namespace GE