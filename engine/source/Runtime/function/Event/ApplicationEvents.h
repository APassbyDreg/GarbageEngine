#pragma once

#include "Event.h"

namespace GE
{

    class GE_API AppTickEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(AppTick, EventCategoryApplication)

        AppTickEvent() {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "AppTickEvent";
            return ss.str();
        }
    };

    class GE_API WindowCloseEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(WindowClose, EventCategoryApplication)

        WindowCloseEvent() {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowCloseEvent";
            return ss.str();
        }
    };

    class GE_API WindowResizeEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(WindowResize, EventCategoryApplication)

        WindowResizeEvent(uint width, uint height) : m_width(width), m_height(height) {}

        inline uint  getWidth() const { return m_width; }
        inline uint  getHeight() const { return m_height; }
        inline uint2 getSize() const { return uint2(m_width, m_height); }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_width << ", " << m_height;
            return ss.str();
        }

    private:
        uint m_width, m_height;
    };

    class GE_API WindowFocusEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(WindowFocus, EventCategoryApplication)

        WindowFocusEvent() {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowFocusEvent";
            return ss.str();
        }
    };

    class GE_API WindowLostFocusEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(WindowLostFocus, EventCategoryApplication)

        WindowLostFocusEvent() {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "WindowLostFocusEvent";
            return ss.str();
        }
    };

} // namespace GE