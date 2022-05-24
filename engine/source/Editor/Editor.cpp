#include "GE_pch.h"

#include "Editor.h"

namespace GE
{
    Editor::Editor(/* args */) {}

    Editor::~Editor() {}

    Application* CreateApplication() { return new Editor(); }
} // namespace GE
