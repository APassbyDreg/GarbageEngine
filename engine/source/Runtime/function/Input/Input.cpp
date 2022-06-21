#include "Input.h"

#include "Application.h"

namespace GE
{
    bool Input::IsKeyPressed(int keycode)
    {
        auto window = Application::GetInstance().GetActiveWindowLayer()->GetNativeWindow();
        auto key_state = glfwGetKey(window, keycode);
        return key_state == GLFW_PRESS || key_state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        auto window = Application::GetInstance().GetActiveWindowLayer()->GetNativeWindow();
        auto mouse_button_state = glfwGetMouseButton(window, button);
        return mouse_button_state == GLFW_PRESS;
    }

    float2 Input::GetMousePosition()
    {
        auto window = Application::GetInstance().GetActiveWindowLayer()->GetNativeWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return float2((float)x, (float)y);
    }
} // namespace GE