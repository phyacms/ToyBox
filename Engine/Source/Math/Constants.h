﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

enum class EAngleUnit : std::size_t
{
	Degree,
	Radian
};

namespace Math
{
	using ValueType = long double;

	struct PI final
	{
		inline static constexpr ValueType Value{ 3.14159265358979323846 };

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline T operator*(T Factor) const noexcept { return static_cast<T>(Value * Factor); }
		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline T operator/(T Divisor) const noexcept { return operator*(T{ 1 } / Divisor); }

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr operator T() const noexcept { return static_cast<T>(Value); }
	};
}

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
inline T operator*(T Factor, Math::PI Pi) noexcept { return Pi * Factor; }