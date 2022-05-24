#pragma once

#include "Event.h"

#include "glm/vec2.hpp"

namespace GE
{
    class GE_API MouseMovedEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(MouseMoved, EventCategoryMouse);

        MouseMovedEvent(float x, float y) : m_xPosition(x), m_yPosition(y) {}

        float     getX() const { return m_xPosition; }
        float     getY() const { return m_yPosition; }
        glm::vec2 getPosition() const { return glm::vec2(m_xPosition, m_yPosition); }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_xPosition << ", " << m_yPosition;
            return ss.str();
        }

    private:
        float m_xPosition, m_yPosition;
    };

    class GE_API MouseScrolledEvent : public Event
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(MouseScrolled, EventCategoryMouse);

        MouseScrolledEvent(float xOffset, float yOffset) : m_xOffset(xOffset), m_yOffset(yOffset) {}

        float     getXOffset() const { return m_xOffset; }
        float     getYOffset() const { return m_yOffset; }
        glm::vec2 getOffset() const { return glm::vec2(m_xOffset, m_yOffset); }

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << m_xOffset << ", " << m_yOffset;
            return ss.str();
        }

    private:
        float m_xOffset, m_yOffset;
    };

    class GE_API MouseButtonEvent : public Event
    {
    public:
        int getMouseButton() const { return m_button; }

    protected:
        MouseButtonEvent(int button) : m_button(button) {}
        int m_button;
    };

    class GE_API MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(MouseButtonPressed, EventCategoryMouse | EventCategoryMouseButton);

        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_button;
            return ss.str();
        }
    };

    class GE_API MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        EVENT_IMPLEMENTATION_COMMON(MouseButtonReleased, EventCategoryMouse | EventCategoryMouseButton);

        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_button;
            return ss.str();
        }
    };

} // namespace GE