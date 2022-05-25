#include "GE_pch.h"

#include "Editor.h"

namespace GE
{
    Editor::Editor() { GE_APP_INFO("Constructing Editor"); }

    Editor::~Editor() {}

    std::unique_ptr<Application> CreateApplication() { return std::make_unique<Editor>(); }
} // namespace GE
