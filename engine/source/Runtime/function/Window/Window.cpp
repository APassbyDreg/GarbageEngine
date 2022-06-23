#include "Window.h"

#include "Runtime/Application.h"

#include "Runtime/function/Event/EventSystem.h"
#include "Runtime/function/Message/MessageSystem.h"
#include "Runtime/function/Render/VulkanManager/VulkanManager.h"

namespace GE
{
    bool Window::s_glfwInitialized = false;

    Window::Window(const WindowProperties& props) { __init(props); }

    Window::~Window() { __shutdown(); }

    void Window::BeginWindowRender()
    {
        glfwPollEvents();

        if (m_needRebuildSwapChain)
        {
            __imgui_rebuild_swapchain();
        }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_imguiContext = ImGui::GetCurrentContext();
    }

    void Window::EndWindowRender()
    {
        // Rendering
        ImGui::Render();
        ImVec4      clear_color      = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImDrawData* main_draw_data   = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        m_imguiWindow.ClearValue.color.float32[0] = clear_color.x * clear_color.w;
        m_imguiWindow.ClearValue.color.float32[1] = clear_color.y * clear_color.w;
        m_imguiWindow.ClearValue.color.float32[2] = clear_color.z * clear_color.w;
        m_imguiWindow.ClearValue.color.float32[3] = clear_color.w;
        if (!main_is_minimized)
            __imgui_render_frame(main_draw_data);

        // Update and Render additional Platform Windows
        if (m_imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present Main Platform Window
        if (!main_is_minimized)
            __imgui_present_frame();
    }

    void Window::__imgui_render_frame(ImDrawData* draw_data)
    {
        VkDevice vk_device         = VulkanManager::GetInstance().GetVkDevice();
        VkQueue  vk_graphics_queue = VulkanManager::GetInstance().GetVkGraphicsQueue();

        VkSemaphore image_acquired_semaphore =
            m_imguiWindow.FrameSemaphores[m_imguiWindow.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore =
            m_imguiWindow.FrameSemaphores[m_imguiWindow.SemaphoreIndex].RenderCompleteSemaphore;
        VkResult res = vkAcquireNextImageKHR(vk_device,
                                             m_imguiWindow.Swapchain,
                                             UINT64_MAX,
                                             image_acquired_semaphore,
                                             VK_NULL_HANDLE,
                                             &m_imguiWindow.FrameIndex);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
        {
            m_needRebuildSwapChain = true;
            return;
        }
        VK_CHECK(res);

        ImGui_ImplVulkanH_Frame* fd = &m_imguiWindow.Frames[m_imguiWindow.FrameIndex];
        {
            VK_CHECK(vkWaitForFences(vk_device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));

            VK_CHECK(vkResetFences(vk_device, 1, &fd->Fence));
        }
        {
            VK_CHECK(vkResetCommandPool(vk_device, fd->CommandPool, 0));
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK(vkBeginCommandBuffer(fd->CommandBuffer, &info));
        }
        {
            VkRenderPassBeginInfo info    = {};
            info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass               = m_imguiWindow.RenderPass;
            info.framebuffer              = fd->Framebuffer;
            info.renderArea.extent.width  = m_imguiWindow.Width;
            info.renderArea.extent.height = m_imguiWindow.Height;
            info.clearValueCount          = 1;
            info.pClearValues             = &m_imguiWindow.ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = 1;
            info.pWaitSemaphores            = &image_acquired_semaphore;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &fd->CommandBuffer;
            info.signalSemaphoreCount       = 1;
            info.pSignalSemaphores          = &render_complete_semaphore;

            VK_CHECK(vkEndCommandBuffer(fd->CommandBuffer));
            VK_CHECK(vkQueueSubmit(vk_graphics_queue, 1, &info, fd->Fence));
        }
    }

    void Window::__imgui_present_frame()
    {
        if (m_needRebuildSwapChain)
        {
            return;
        }

        VkDevice vk_device         = VulkanManager::GetInstance().GetVkDevice();
        VkQueue  vk_graphics_queue = VulkanManager::GetInstance().GetVkGraphicsQueue();

        VkSemaphore render_complete_semaphore =
            m_imguiWindow.FrameSemaphores[m_imguiWindow.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info   = {};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores    = &render_complete_semaphore;
        info.swapchainCount     = 1;
        info.pSwapchains        = &m_imguiWindow.Swapchain;
        info.pImageIndices      = &m_imguiWindow.FrameIndex;

        VkResult res = vkQueuePresentKHR(vk_graphics_queue, &info);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
        {
            m_needRebuildSwapChain = true;
            return;
        }
        VK_CHECK(res);

        m_imguiWindow.SemaphoreIndex =
            (m_imguiWindow.SemaphoreIndex + 1) % m_imguiWindow.ImageCount; // Now we can use the next set of semaphores
    }

    void Window::__imgui_rebuild_swapchain()
    {
        VkInstance       vk_instance             = VulkanManager::GetInstance().GetVkInstance();
        VkPhysicalDevice vk_physical_device      = VulkanManager::GetInstance().GetVkPhysicalDevice();
        VkDevice         vk_device               = VulkanManager::GetInstance().GetVkDevice();
        uint32_t         vk_graphics_queue_index = VulkanManager::GetInstance().GetVkGraphicsQueueFamilyIndex();

        int width, height;
        glfwGetFramebufferSize(m_glfwWindow, &width, &height);

        if (width > 0 && height > 0)
        {
            ImGui_ImplVulkan_SetMinImageCount(2); // TODO: rethink this value
            ImGui_ImplVulkanH_CreateOrResizeWindow(vk_instance,
                                                   vk_physical_device,
                                                   vk_device,
                                                   &m_imguiWindow,
                                                   vk_graphics_queue_index,
                                                   nullptr,
                                                   width,
                                                   height,
                                                   2);
            m_imguiWindow.FrameIndex = 0;
            m_needRebuildSwapChain   = false;
        }
    }

    void Window::__init_glfw()
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
        m_glfwWindow = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
        GE_CORE_ASSERT(m_glfwWindow, "Could not create window!");

        glfwSetWindowUserPointer(m_glfwWindow, &m_Data);
    }

    void Window::__cleanup_glfw()
    {
        if (s_glfwInitialized)
        {
            glfwDestroyWindow(m_glfwWindow);
            m_glfwWindow = nullptr;
        }
        s_glfwInitialized = false;
        glfwTerminate(); // REVIEW: is this necessary?
    }

    void Window::__init_glfw_callbacks()
    {
        // window callbalcks
        glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width       = width;
            data.height      = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });
        glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.eventCallback(event);
        });
        glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow* window, int focused) {
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
        glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event(xpos, ypos);
            data.eventCallback(event);
        });
        glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
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
        glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event(xoffset, yoffset);
            data.eventCallback(event);
        });
        // key callbacks
        glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
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

    void Window::__init_imgui(int2 size)
    {
        VkInstance       vk_instance             = VulkanManager::GetInstance().GetVkInstance();
        VkSurfaceKHR     vk_surface              = VulkanManager::GetInstance().GetVkSurface();
        VkPhysicalDevice vk_physical_device      = VulkanManager::GetInstance().GetVkPhysicalDevice();
        VkDevice         vk_device               = VulkanManager::GetInstance().GetVkDevice();
        VkQueue          vk_graphics_queue       = VulkanManager::GetInstance().GetVkGraphicsQueue();
        uint32_t         vk_graphics_queue_index = VulkanManager::GetInstance().GetVkGraphicsQueueFamilyIndex();

        m_imguiWindow.Surface = vk_surface;

        // Check for WSI support
        {
            VkBool32 res;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
                vk_physical_device, vk_graphics_queue_index, m_imguiWindow.Surface, &res));
            GE_CORE_ASSERT(res == VK_TRUE, "WSI not supported Error no WSI support on selected physical device!");
        }

        // Select Surface Format
        {
            const VkFormat requestSurfaceImageFormat[] = {
                VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
            const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

            m_imguiWindow.SurfaceFormat =
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
            m_imguiWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
                vk_physical_device, m_imguiWindow.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        }

        // Create SwapChain, RenderPass, Framebuffer, etc.
        {
            ImGui_ImplVulkanH_CreateOrResizeWindow(vk_instance,
                                                   vk_physical_device,
                                                   vk_device,
                                                   &m_imguiWindow,
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
            m_imguiIO = ImGui::GetIO();
            m_imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            m_imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
            m_imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

            // Setup Dear ImGui style
            // ImGui::StyleColorsDark();
            ImGui::StyleColorsClassic();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
            // regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (m_imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding              = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
        }

        // setup descriptor pool
        {
            VkDescriptorPoolSize       pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
            VkDescriptorPoolCreateInfo pool_info    = {};
            pool_info.sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags                         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets                       = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount                 = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes                    = pool_sizes;
            VK_CHECK(vkCreateDescriptorPool(vk_device, &pool_info, nullptr, &m_imguiDescriptorPool));
        }

        // Setup Platform/Renderer backends
        {
            ImGui_ImplGlfw_InitForVulkan(m_glfwWindow, true);
            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance                  = vk_instance;
            init_info.PhysicalDevice            = vk_physical_device;
            init_info.Device                    = vk_device;
            init_info.QueueFamily               = vk_graphics_queue_index;
            init_info.Queue                     = vk_graphics_queue;
            init_info.PipelineCache             = m_imguiPipelineCache;
            init_info.DescriptorPool            = m_imguiDescriptorPool;
            init_info.Subpass                   = 0;
            init_info.MinImageCount             = 2;
            init_info.ImageCount                = m_imguiWindow.ImageCount;
            init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator                 = nullptr;
            init_info.CheckVkResultFn           = __vk_check_fn;
            ImGui_ImplVulkan_Init(&init_info, m_imguiWindow.RenderPass);
        }

        // Upload Fonts
        {
            // Use any command queue
            VkCommandPool   command_pool   = m_imguiWindow.Frames[m_imguiWindow.FrameIndex].CommandPool;
            VkCommandBuffer command_buffer = m_imguiWindow.Frames[m_imguiWindow.FrameIndex].CommandBuffer;

            VK_CHECK(vkResetCommandPool(vk_device, command_pool, 0));
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info));

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info       = {};
            end_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers    = &command_buffer;

            VK_CHECK(vkEndCommandBuffer(command_buffer));
            VK_CHECK(vkQueueSubmit(vk_graphics_queue, 1, &end_info, VK_NULL_HANDLE));
            VK_CHECK(vkDeviceWaitIdle(vk_device));

            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void Window::__cleanup_imgui()
    {
        ImGui_ImplVulkanH_DestroyWindow(VulkanManager::GetInstance().GetVkInstance(),
                                        VulkanManager::GetInstance().GetVkDevice(),
                                        &m_imguiWindow,
                                        nullptr);
    }

    void Window::__init(const WindowProperties& props)
    {
        m_Data.title         = props.title;
        m_Data.width         = props.width;
        m_Data.height        = props.height;
        m_Data.eventCallback = props.eventCallback;

        // setup glfw
        __init_glfw();

        // setup callbacks
        __init_glfw_callbacks();

        // setup vulkan
        VulkanManager::GetInstance().Init(m_glfwWindow);

        // setup imgui
        __init_imgui(int2(props.width, props.height));
    }

    void Window::__shutdown()
    {
        __cleanup_imgui();
        __cleanup_glfw();
    }

    void Window::SetTitle(const std::string& title)
    {
        m_Data.title = title;
        glfwSetWindowTitle(m_glfwWindow, title.c_str());
    }
} // namespace GE