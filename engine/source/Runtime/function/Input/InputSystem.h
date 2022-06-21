#pragma once

#include "GE_pch.h"

#include "Runtime/core/base/Singleton.h"
#include "Runtime/core/math/math.h"

#include "InputCodes.h"

namespace GE
{
    class GE_API InputSystem
    {
    public:
        static bool   IsKeyPressed(const KeyCode keycode);
        static bool   IsMouseButtonPressed(const MouseCode button);
        static float2 GetMousePosition();
    };

} // namespace GE