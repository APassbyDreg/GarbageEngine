#pragma once

#include "ApplicationEvents.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "KeyEvents.h"
#include "MouseEvents.h"

namespace GE
{
    typedef std::function<void(Event&)> EventCallbackFn;
}