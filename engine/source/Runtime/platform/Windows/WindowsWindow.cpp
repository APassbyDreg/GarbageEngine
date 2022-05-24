#include "WindowsWindow.h"

namespace GE
{
    bool WindowsWindow::s_GLFWInitialized = false;

    WindowsWindow::WindowsWindow(const WindowProperties& props) { Init(props); }

    WindowsWindow::~WindowsWindow() { Shutdown(); }

    void WindowsWindow::OnTick()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void WindowsWindow::Init(const WindowProperties& props)
    {
        m_Data.title  = props.title;
        m_Data.width  = props.width;
        m_Data.height = props.height;

        if (!s_GLFWInitialized)
        {
            // TODO: glfwTerminate on system shutdown
            int success = glfwInit();
            GE_CORE_ASSERT(success, "Could not initialize GLFW!");

            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
        GE_CORE_ASSERT(m_Window, "Could not create window!");

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);

        // VSync is always on
        glfwSwapInterval(1);
    }

    void WindowsWindow::Shutdown() { glfwDestroyWindow(m_Window); }

    // implement window creation
    Window* Window::Create(const WindowProperties& props) { return new WindowsWindow(props); }
} // namespace GE