#define GE_ENTRYPOINT_CPP
#include "Editor.h"

#include "Explorer.h"
#include "Menu.h"
#include "Test.h"

#include "MaterialInspector.h"
#include "MeshInspector.h"
#include "SceneInspector.h"

namespace GE
{
    Editor::Editor()
    {
        GE_APP_INFO("Constructing Editor");

        auto test_layer = std::make_shared<TestLayer>();
        PushLayer(test_layer);

        auto menu_layer = std::make_shared<MenuLayer>();
        PushLayer(menu_layer);

        auto explorer_layer = std::make_shared<ExplorerLayer>();
        PushLayer(explorer_layer);

        auto scene_inspector_layer = std::make_shared<SceneInspectorLayer>();
        PushLayer(scene_inspector_layer);

        auto material_inspector_layer = std::make_shared<MaterialInspectorLayer>();
        PushLayer(material_inspector_layer);

        auto mesh_inspector_layer = std::make_shared<MeshInspectorLayer>();
        PushLayer(mesh_inspector_layer);
    }

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
