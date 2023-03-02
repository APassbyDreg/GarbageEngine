#pragma once

#include "GE_pch.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Runtime/core/Math/Math.h"
#include "Runtime/function/Event/EventSystem.h"

#include "Runtime/function/Render/Routines/BuiltinRenderRoutine.h"

namespace GE
{
    struct WindowProperties
    {
        std::string     title;
        uint            width, height;
        EventCallbackFn eventCallback;

        WindowProperties(const std::string& title         = "GE Engine",
                         uint               width         = 1920,
                         uint               height        = 1080,
                         EventCallbackFn    eventCallback = nullptr) :
            title(title),
            width(width), height(height), eventCallback(eventCallback)
        {}
    };

    struct WindowData
    {
        std::string  title;
        unsigned int width, height;

        EventCallbackFn eventCallback;
    };

    class GE_API Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window(const WindowProperties& props);
        virtual ~Window();

        virtual void SetEventCallback(const EventCallbackFn& callback) { m_Data.eventCallback = callback; };
        virtual uint GetWidth() const { return m_Data.width; }
        virtual uint GetHeight() const { return m_Data.height; }
        inline uint2 GetSize() const { return uint2(GetWidth(), GetHeight()); }

        void                 BeginWindowRender();
        void                 EndWindowRender();
        inline ImGuiContext* GetImGuiContext() { return m_imguiContext; }

        void               SetTitle(const std::string& title);
        inline GLFWwindow* GetNativeWindow() const { return m_glfwWindow; }

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

        void __viewport_resize(ImVec2 size);

    protected:
        /* --------------------------- glfw stuff --------------------------- */

        static bool s_glfwInitialized;
        GLFWwindow* m_glfwWindow;
        WindowData  m_Data;

        /* --------------------------- imgui stuff -------------------------- */
        ImGuiIO*                 m_imguiIO;
        ImGui_ImplVulkanH_Window m_imguiWindow;
        ImGuiContext*            m_imguiContext;

        // TODO: move to unified management
        VkPipelineCache  m_imguiPipelineCache  = VK_NULL_HANDLE;
        VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;

        // TODO: rethink where this should be placed
        float m_Time                 = 0.0f;
        bool  m_needRebuildSwapChain = false;

        /* ------------------------- render routine ------------------------- */
        // TODO: move to dedicate manager
        int                  m_usingRenderRoutine = 0;
        BuiltinRenderRoutine m_renderRoutine0;

        VkSampler                                 m_viewportSampler;
        std::vector<std::vector<VkDescriptorSet>> m_viewportDescriptorSets;
        std::vector<VkSemaphore>                  m_renderFinishedSemaphores;
        ImVec2                                    m_viewportSize = {1280, 720};

        uint64 m_frameIdx = 0;
    };
} // namespace GE