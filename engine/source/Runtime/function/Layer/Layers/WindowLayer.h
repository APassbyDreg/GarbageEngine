#pragma once

#include "GE_pch.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "core/math/math.h"
#include "function/Event/EventSystem.h"
#include "function/Layer/LayerSystem.h"

#include "function/UI/Window.h"

namespace GE
{
    struct WindowData
    {
        std::string  title;
        unsigned int width, height;

        EventCallbackFn eventCallback;
    };

    class GE_API WindowLayer : public Layer
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        WindowLayer(const WindowProperties& props);
        virtual ~WindowLayer();

        inline GLFWwindow* GetNativeWindow() const { return m_glfwWindow; }

        virtual void SetEventCallback(const EventCallbackFn& callback) { m_Data.eventCallback = callback; };
        virtual uint GetWidth() const { return m_Data.width; }
        virtual uint GetHeight() const { return m_Data.height; }
        inline uint2 GetSize() const { return uint2(GetWidth(), GetHeight()); }

        /* ---------------------- override layer stuff ---------------------- */

        virtual void OnEnable() override;
        virtual void OnDisable() override;
        virtual void OnAttach() override;
        virtual void OnDetatch() override;
        virtual void OnUpdate() override;
        virtual void OnEvent(Event& event) override;

    private:
        /* ------------------------- private helpers ------------------------ */

        void __init(const WindowProperties& props);
        void __shutdown();

        void __init_glfw();
        void __init_imgui(int2 size);

        void __cleanup_glfw();
        void __cleanup_imgui();

        void __init_glfw_callbacks();

        void __imgui_render_frame(ImDrawData* draw_data);
        void __imgui_present_frame();
        void __imgui_rebuild_swapchain();

    protected:

        /* --------------------------- glfw stuff --------------------------- */

        static bool s_glfwInitialized;
        GLFWwindow* m_glfwWindow;
        WindowData  m_Data;

        /* --------------------------- imgui stuff -------------------------- */
        ImGuiIO                  m_imguiIO;
        ImGui_ImplVulkanH_Window m_imguiWindow;

        // TODO: move to unified management
        VkPipelineCache  m_imguiPipelineCache  = VK_NULL_HANDLE;
        VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;

        // TODO: rethink where this should be placed
        float m_Time                 = 0.0f;
        bool  m_needRebuildSwapChain = false;
    };
} // namespace GE