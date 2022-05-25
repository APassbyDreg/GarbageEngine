#include "GE_pch.h"

#include "Editor.h"

namespace GE
{
    Editor::Editor(/* args */) {}

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
