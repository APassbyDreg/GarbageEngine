#pragma once

#include "GE_pch.h"

#include "core/math/math.h"
#include "function/Event/EventSystem.h"

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
        inline uint2 GetSize() const { return uint2(GetWidth(), GetHeight()); }

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        static std::unique_ptr<Window> Create(const WindowProperties& properties = WindowProperties());
    };
} // namespace GE