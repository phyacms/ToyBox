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
#define USTR(x) u#x

// Tweaks
// @NOTE: https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/
#if _MSC_VER >= 1929 // VS2019 v16.10 and later
#define no_unique_address msvc::no_unique_address
#endif
