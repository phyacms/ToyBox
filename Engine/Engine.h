﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

// C++ Standard Headers
#include <cstdlib>
#include <cstdint>
#include <charconv>
#include <cmath>
#include <limits>
#include <chrono>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <unordered_set>
#include <queue>
#include <optional>
#include <variant>
#include <functional>
#include <utility>
#include <execution>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <type_traits>
#include <stdexcept>

// Common Setups
namespace stdhelp
{
	template<typename... Ts>
	class overloaded final
		: public Ts...
	{
	public:
		using Ts::operator()...;
	};
}

// Macros
#define __USTR(X) u##X
#define USTR(x) __USTR(x)

#define BeginConstexprIf() if constexpr (false) {}
#define ConstexprIfCase(Condition) else if constexpr ((Condition))
#define EndConstexprIf() else { static_assert(false); }

// Tweaks
// @NOTE: https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/
#if _MSC_VER >= 1929 // VS2019 v16.10 and later
#define no_unique_address msvc::no_unique_address
#endif
