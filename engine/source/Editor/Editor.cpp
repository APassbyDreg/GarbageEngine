#define GE_ENTRYPOINT_CPP
#include "Editor.h"

namespace GE
{
    Editor::Editor()
    {
        GE_APP_INFO("Constructing Editor");

        std::shared_ptr<TestLayer> test_layer = std::make_shared<TestLayer>();
        PushLayer(test_layer);

        std::shared_ptr<InspectorLayer> inspector_layer = std::make_shared<InspectorLayer>();
        PushLayer(inspector_layer);
    }

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
