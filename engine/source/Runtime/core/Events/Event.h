#pragma once

#include "GE_pch.h"

#include "../../macros/macros.h"
#include "../LogSystem.h"

namespace GE
{
    enum class EventType
    {
        None = 0,
        // keyboard events
        KeyPressed,
        KeyReleased,
        KeyTyped,
        // mouse events
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled,
        // application events
        AppTick,
        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
    };

    enum EventCategory
    {
        EventCategoryNone        = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput       = BIT(1),
        EventCategoryKeyboard    = BIT(2),
        EventCategoryMouse       = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

    /**
     * @brief event base class
     *
     */
    class GE_API Event
    {
    public:
        virtual EventType   getEventType() const     = 0;
        virtual const char* getName() const          = 0;
        virtual int         getCategoryFlags() const = 0;
        virtual std::string toString() const { return ""; }

        inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }

    private:
        bool m_handled = false;
    };
} // namespace GE

// event final class shorthand macro
#define EVENT_IMPLEMENTATION_COMMON(type, category) \
    EventType   getEventType() const override { return EventType::type; } \
    const char* getName() const override { return #type; } \
    int         getCategoryFlags() const override { return category; }