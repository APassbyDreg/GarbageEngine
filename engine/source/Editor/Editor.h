#pragma once

#include "GE.h"

#include "imgui.h"

namespace GE
{
    class TestLayer : public Layer
    {
    public:
        TestLayer() {}
        ~TestLayer() {}

        void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);

            ImGui::Begin("Test");
            ImGui::Text("This is a test layer");
            ImGui::End();
        }
    };

    class Editor : public Application
    {
    public:
        Editor();
        ~Editor();
    };

} // namespace GE
