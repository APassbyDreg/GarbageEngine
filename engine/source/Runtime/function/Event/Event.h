#pragma once

#include "GE_pch.h"

#include "Runtime/core/Math/Math.h"

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
        virtual EventType   GetEventType() const     = 0;
        virtual const char* GetName() const          = 0;
        virtual int         GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return ""; }

        inline bool isInCategory(EventCategory category) { return GetCategoryFlags() & category; }

        template<typename OStream>
        friend OStream& operator<<(OStream& os, const Event& c)
        {
            return os << ToString();
        }

    public:
        bool m_handled = false;
    };

    template<std::derived_from<Event> EventT, typename CharT>
    struct std::formatter<EventT, CharT> : std::formatter<std::string>
    {
        template<typename FormatContext>
        auto format(EventT& e, FormatContext& ctx) const
        {
            std::string name = e.ToString();
            return std::formatter<std::string>::format(name, ctx);
        }
    };

// event final class shorthand macro
#define EVENT_IMPLEMENTATION_COMMON(type, category, name) \
    static EventType GetStaticType() { return EventType::type; } \
    EventType        GetEventType() const override { return EventType::type; } \
    const char*      GetName() const override { return #name; } \
    int              GetCategoryFlags() const override { return category; }
} // namespace GE
