/**
 * @file GE_pch.h
 * @brief precompiled header
 * @date 2022-05-24
 */

#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Runtime/macros/macros.h"

#ifdef GE_PLATFORM_WINDOWS
#include "Windows.h"
#endif