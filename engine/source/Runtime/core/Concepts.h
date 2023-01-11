#pragma once

#include "GE_pch.h"

namespace GE
{
    template<typename T, typename... U>
    concept IsAnyOf = (std::same_as<T, U> || ...);
} // namespace GE