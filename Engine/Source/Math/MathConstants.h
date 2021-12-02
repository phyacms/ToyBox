// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

enum class EAngleUnit : std::size_t
{
	Degree,
	Radian
};

namespace MathConstants
{
	using ValueType = long double;

	struct PI final
	{
		inline static constexpr ValueType Value{ 3.14159265358979323846 };
		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr operator T() const noexcept { return static_cast<T>(Value); }
	};
}
