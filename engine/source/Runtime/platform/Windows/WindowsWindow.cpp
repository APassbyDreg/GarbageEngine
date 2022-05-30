#include "WindowsWindow.h"

#include "function/Event/EventSystem.h"

#include "function/Render/VulkanManager/VulkanManager.h"

namespace GE
{
    bool                     WindowsWindow::s_glfwInitialized = false;
    ImGui_ImplVulkanH_Window WindowsWindow::s_imguiWindow;

    WindowsWindow::WindowsWindow(const WindowProperties& props) { Init(props); }

    WindowsWindow::~WindowsWindow() { Shutdown(); }

    void WindowsWindow::OnUpdate() { glfwPollEvents(); }

    void WindowsWindow::init_glfw()
    {
        if (!s_glfwInitialized)
        {
            // TODO: move to dedicated function
            glfwSetErrorCallback(
                [](int error, const char* description) { GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description); });
            s_glfwInitialized = true;

            // TODO: glfwTerminate on system shutdown
            GE_CORE_ASSERT(glfwInit(), "Could not initialize GLFW!");
        }

        // set hint
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // create window
        m_window = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
        GE_CORE_ASSERT(m_window, "Could not create window!");

        glfwSetWindowUserPointer(m_window, &m_Data);
    }

    void WindowsWindow::cleanup_glfw()
    {
        if (s_glfwInitialized)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        s_glfwInitialized = false;
    }

    void WindowsWindow::init_glfw_callbacks()
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

    void WindowsWindow::init_imgui(int2 size)
    {
        VkInstance       vk_instance             = VulkanManager::GetInstance().GetVkInstance();
        VkSurfaceKHR     vk_surface              = VulkanManager::GetInstance().GetVkSurface();
        VkPhysicalDevice vk_physical_device      = VulkanManager::GetInstance().GetVkPhysicalDevice();
        VkDevice         vk_device               = VulkanManager::GetInstance().GetVkDevice();
        VkQueue          vk_graphics_queue       = VulkanManager::GetInstance().GetVkGraphicsQueue();
        uint32_t         vk_graphics_queue_index = VulkanManager::GetInstance().GetVkGraphicsQueueFamilyIndex();

        s_imguiWindow.Surface = vk_surface;

        // Check for WSI support
        {
            VkBool32 res;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
                vk_physical_device, vk_graphics_queue_index, s_imguiWindow.Surface, &res));
            GE_CORE_ASSERT(res == VK_TRUE, "WSI not supported Error no WSI support on selected physical device!");
        }

        // Select Surface Format
        {
            const VkFormat requestSurfaceImageFormat[] = {
                VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
            const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

            s_imguiWindow.SurfaceFormat =
                ImGui_ImplVulkanH_SelectSurfaceFormat(vk_physical_device,
                                                      vk_surface,
                                                      requestSurfaceImageFormat,
                                                      (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                      requestSurfaceColorSpace);
        }

        // Select Present Mode
        {
            VkPresentModeKHR present_modes[] = {
                VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
            s_imguiWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
                vk_physical_device, s_imguiWindow.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        }

        // Create SwapChain, RenderPass, Framebuffer, etc.
        {
            ImGui_ImplVulkanH_CreateOrResizeWindow(vk_instance,
                                                   vk_physical_device,
                                                   vk_device,
                                                   &s_imguiWindow,
                                                   vk_graphics_queue_index,
                                                   nullptr,
                                                   size.x,
                                                   size.y,
                                                   2);
        }

        // basic configs
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
            // io.ConfigViewportsNoAutoMerge = true;
            // io.ConfigViewportsNoTaskBarIcon = true;

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
            // regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding              = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
        }

        // Setup Platform/Renderer backends
        // {
        //     ImGui_ImplGlfw_InitForVulkan(window, true);
        //     ImGui_ImplVulkan_InitInfo init_info = {};
        //     init_info.Instance                  = vk_instance;
        //     init_info.PhysicalDevice            = vk_physical_device;
        //     init_info.Device                    = vk_device;
        //     init_info.QueueFamily               = vk_graphics_queue_index;
        //     init_info.Queue                     = vk_graphics_queue;
        //     init_info.PipelineCache             = g_PipelineCache;
        //     init_info.DescriptorPool            = g_DescriptorPool;
        //     init_info.Subpass                   = 0;
        //     init_info.MinImageCount             = 2;
        //     init_info.ImageCount                = s_imguiWindow.ImageCount;
        //     init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
        //     init_info.Allocator                 = nullptr;
        //     init_info.CheckVkResultFn           = __vk_check_fn;
        //     ImGui_ImplVulkan_Init(&init_info, s_imguiWindow.RenderPass);
        // }
    }

    void WindowsWindow::cleanup_imgui()
    {
        ImGui_ImplVulkanH_DestroyWindow(VulkanManager::GetInstance().GetVkInstance(),
                                        VulkanManager::GetInstance().GetVkDevice(),
                                        &s_imguiWindow,
                                        nullptr);
    }

    void WindowsWindow::Init(const WindowProperties& props)
    {
        m_Data.title  = props.title;
        m_Data.width  = props.width;
        m_Data.height = props.height;

        // setup glfw
        init_glfw();

        // setup callbacks
        init_glfw_callbacks();

        // setup vulkan
        VulkanManager::GetInstance().Init(m_window);

        // setup imgui
        init_imgui(int2(props.width, props.height));
    }

    void WindowsWindow::Shutdown()
    {
        cleanup_imgui();
        cleanup_glfw();
    }

    // implement window creation
    std::shared_ptr<Window> Window::Create(const WindowProperties& props)
    {
        return std::make_shared<WindowsWindow>(props);
    }
} // namespace GE