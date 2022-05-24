#pragma once

#include "Event.h"

namespace GE
{
    class GE_API KeyEvent : public Event
    {
    public:
        int getKeyCode() const { return m_keyCode; }

    protected:
        KeyEvent(int keyCode) : m_keyCode(keyCode) {}

        int m_keyCode;
    };

    class GE_API KeyPressedEvent : public KeyEvent
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(KeyPressed, EventCategoryKeyboard)

        KeyPressedEvent(int keyCode, int repeatCount = 0) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}

        int getRepeatCount() const { return m_repeatCount; }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
            return ss.str();
        }

    private:
        int m_repeatCount = 0;
    };

    class GE_API KeyReleasedEvent : public KeyEvent
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(KeyReleased, EventCategoryKeyboard)

        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}
    };
} // namespace GE