#pragma once

#include "GE_pch.h"

#include "Events/Event.h"

#include "glm/vec2.hpp"

namespace GE
{
    struct WindowProperties
    {
        std::string title;
        uint        width, height;

        WindowProperties(const std::string& title = "GE Engine", uint width = 1280, uint height = 720) :
            title(title), width(width), height(height)
        {}
    };

    /**
     * @brief interface representing a window
     *
     */
    class GE_API Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnTick()          = 0;
        virtual uint GetWidth() const  = 0;
        virtual uint GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        static Window* Create(const WindowProperties& properties = WindowProperties());
    };
} // namespace GE