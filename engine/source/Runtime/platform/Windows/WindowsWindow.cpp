#include "WindowsWindow.h"

#include "function/Event/EventSystem.h"

namespace GE
{
    bool WindowsWindow::s_GLFWInitialized = false;

    WindowsWindow::WindowsWindow(const WindowProperties& props) { Init(props); }

    WindowsWindow::~WindowsWindow() { Shutdown(); }

    void WindowsWindow::OnTick()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
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

            // TODO: move to dedicated function
            glfwSetErrorCallback(
                [](int error, const char* description) { GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description); });
            s_GLFWInitialized = true;
        }

        m_window = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
        GE_CORE_ASSERT(m_window, "Could not create window!");

        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, &m_Data);

        // VSync is always on
        glfwSwapInterval(1);

        // setup callbacks
        InitCallbacks();
    }

    void WindowsWindow::InitCallbacks()
    {
        // window callbalcks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width       = width;
            data.height      = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.eventCallback(event);
        });
        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            if (focused == GLFW_TRUE)
            {
                WindowFocusEvent event;
                data.eventCallback(event);
            }
            else if (focused == GLFW_FALSE)
            {
                WindowLostFocusEvent event;
                data.eventCallback(event);
            }
        });
        // mouse callbacks
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event(xpos, ypos);
            data.eventCallback(event);
        });
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.eventCallback(event);
                    break;
                }
            }
        });
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event(xoffset, yoffset);
            data.eventCallback(event);
        });
        // key callbacks
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, 1);
                    data.eventCallback(event);
                    break;
                }
            }
        });
    }

    void WindowsWindow::Shutdown() { glfwDestroyWindow(m_window); }

    // implement window creation
    std::unique_ptr<Window> Window::Create(const WindowProperties& props)
    {
        return std::make_unique<WindowsWindow>(props);
    }
} // namespace GE