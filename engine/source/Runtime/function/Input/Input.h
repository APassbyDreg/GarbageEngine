#pragma once

#include "GE_pch.h"

#include "core/base/Singleton.h"
#include "core/math/math.h"

namespace GE
{
    class GE_API Input : public Singleton<Input>
    {
    public:
        static bool   IsKeyPressed(int keycode);
        static bool   IsMouseButtonPressed(int button);
        static float2 GetMousePosition();
    };

} // namespace GE