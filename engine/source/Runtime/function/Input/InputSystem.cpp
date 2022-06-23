#include "InputSystem.h"

#include "Runtime/Application.h"

namespace GE
{
    bool InputSystem::IsKeyPressed(const KeyCode keycode)
    {
        auto window    = Application::GetInstance().GetActiveWindow()->GetNativeWindow();
        auto key_state = glfwGetKey(window, keycode);
        return key_state == GLFW_PRESS || key_state == GLFW_REPEAT;
    }

    bool InputSystem::IsMouseButtonPressed(const MouseCode button)
    {
        auto window             = Application::GetInstance().GetActiveWindow()->GetNativeWindow();
        auto mouse_button_state = glfwGetMouseButton(window, button);
        return mouse_button_state == GLFW_PRESS;
    }

    float2 InputSystem::GetMousePosition()
    {
        auto   window = Application::GetInstance().GetActiveWindow()->GetNativeWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return float2(x, y);
    }
} // namespace GE