#pragma once

#include "GE.h"

#include "imgui.h"

namespace GE
{
    class InspectorLayer : public Layer
    {
    public:
        InspectorLayer() {}
        ~InspectorLayer() {}

        void OnImGuiRender(ImGuiContext* ctx) override
        {
            ImGui::SetCurrentContext(ctx);
			
            Application&           app = Application::GetInstance();
            std::shared_ptr<Scene> sc  = app.GetActiveScene();

            ImGui::Begin("Scene Inspector");
            sc->InspectStructure();
            ImGui::End();

            ImGui::Begin("Entity Inspector");
            sc->InspectFocusedEntity();
            ImGui::End();
        }
    };
} // namespace GE