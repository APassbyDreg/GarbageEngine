#define GE_ENTRYPOINT_CPP
#include "Editor.h"

#include "Explorer.h"
#include "Inspector.h"

namespace GE
{
    Editor::Editor()
    {
        GE_APP_INFO("Constructing Editor");

        std::shared_ptr<TestLayer> test_layer = std::make_shared<TestLayer>();
        PushLayer(test_layer);

        std::shared_ptr<MenuLayer> menu_layer = std::make_shared<MenuLayer>();
        PushLayer(menu_layer);

        std::shared_ptr<InspectorLayer> inspector_layer = std::make_shared<InspectorLayer>();
        PushLayer(inspector_layer);

        std::shared_ptr<ExplorerLayer> explorer_layer = std::make_shared<ExplorerLayer>();
        PushLayer(explorer_layer);
    }

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
