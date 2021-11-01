// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

// C++ Standard Headers
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <memory>
#include <type_traits>
#include <stdexcept>

// Common Setups
namespace stdhelp
{
	template<typename... Ts>
	class overloaded final
		: private Ts...
	{
	public:
		using Ts::operator()...;
	};
}

// Macros
#define __USTR(X) u##X
#define USTR(x) __USTR(x)

// Tweaks
// @NOTE: https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/
#if _MSC_VER >= 1929 // VS2019 v16.10 and later
#define no_unique_address msvc::no_unique_address
#endif
