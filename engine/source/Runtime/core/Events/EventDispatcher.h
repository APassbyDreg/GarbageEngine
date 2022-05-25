#pragma once

#include "Event.h"

namespace GE
{
    /**
     * @brief Dispatch events to functions
     * @note usage: create dispatcher with event and call Dispatcher::Dispatch() with functions
     */
    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event) : m_event(event) {}

        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (m_event.getEventType() == T::getStaticType())
            {
                m_event.m_handled = func(static_cast<T&>(m_event));
                return true;
            }
            return false;
        }

    private:
        Event& m_event;
    };
} // namespace GE