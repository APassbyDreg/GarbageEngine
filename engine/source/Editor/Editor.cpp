#include "Editor.h"

namespace GE
{
    Editor::Editor()
    {
        GE_APP_INFO("Constructing Editor");
        std::shared_ptr<TestLayer> layer = std::make_shared<TestLayer>();
        PushLayer(layer);
    }

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
