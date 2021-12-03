﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Functions.h"

enum class EAngleUnit : std::size_t
{
	Degree,
	Radian
};

namespace Math
{
	struct PI final
	{
		using ValueType = long double;
		inline static constexpr ValueType Value{ 3.14159265358979323846 };

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr operator T() const noexcept { return static_cast<T>(Value); }
	};

	class FConvertAngleUnit final
	{
	public:
		inline static constexpr auto DegToRad{ Math::PI{}.Value / 180 };
		inline static constexpr auto RadToDeg{ 180 / Math::PI{}.Value };

	public:
		template<typename T>
		inline T operator()(T Angle, EAngleUnit Unit) noexcept
		{
			switch (Unit)
			{
				case EAngleUnit::Degree: return static_cast<T>(Angle * DegToRad);
				case EAngleUnit::Radian: return static_cast<T>(Angle * RadToDeg);
				default: return {};
			}
		}
	};

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline T ToDegree(T Radian) noexcept { return FConvertAngleUnit{}(Radian, EAngleUnit::Radian); }

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline T ToRadian(T Degree) noexcept { return FConvertAngleUnit{}(Degree, EAngleUnit::Degree); }
}

template<typename ValueType>
class TAngle final
{
public:
	using ImplType = Math::PI::ValueType;
	inline static constexpr auto DefaultUnit{ EAngleUnit::Radian };

private:
	inline static constexpr auto Pi2{ 2 * Math::PI{}.Value };

public:
	TAngle(ImplType Angle = {}, EAngleUnit Unit = DefaultUnit)
		: Radian{
			Unit == EAngleUnit::Radian
			? Angle
			: Math::ToRadian(Angle) } {}
	TAngle(Math::PI Pi) : Radian{ Pi.Value } {}
	TAngle(const TAngle&) noexcept = default;
	TAngle& operator=(const TAngle&) & noexcept = default;
	TAngle(TAngle&&) noexcept = default;
	TAngle& operator=(TAngle&&) & noexcept = default;
	~TAngle() noexcept = default;

public:
	inline TAngle operator+() const noexcept { return Radian; }
	inline TAngle operator-() const noexcept { return -Radian; }

	inline TAngle operator+(const TAngle& Angle) const noexcept { return Radian + Angle.Radian; }
	inline TAngle operator-(const TAngle& Angle) const noexcept { return Radian - Angle.Radian; }
	inline TAngle operator*(ImplType Factor) const noexcept { return Radian * Factor; }
	inline TAngle operator/(ImplType Divisor) const noexcept { return Radian / Divisor; }

	inline TAngle& operator+=(const TAngle& Angle) noexcept { return *this = operator+(Angle); }
	inline TAngle& operator-=(const TAngle& Angle) noexcept { return *this = operator-(Angle); }
	inline TAngle& operator*=(ImplType Factor) noexcept { return *this = operator*(Factor); }
	inline TAngle& operator/=(ImplType Divisor) noexcept { return *this = operator/(Divisor); }

	template<typename T = ValueType, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline operator TAngle<T>() const noexcept { return TAngle<T>{ static_cast<T>(Radian) }; }
	template<typename T = ValueType, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline operator T() const noexcept { return static_cast<T>(Radian); }

public:
	inline bool IsZero() const noexcept { return Math::IsEqualTo<ImplType>(Normalized().Radian, 0); }

	inline bool IsNormalized() const noexcept { return Math::IsEqualTo(Radian, Normalized().Radian); }
	inline TAngle Normalized() const noexcept { return TAngle{ *this }.Normalize(); }
	inline TAngle& Normalize() noexcept
	{
		while (Radian < 0) { Radian += Pi2; }
		while (Radian > Pi2) { Radian -= Pi2; }
	}

	template<typename T = ValueType, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline T ToRadian() const noexcept { return static_cast<T>(Radian); }
	template<typename T = ValueType, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline T ToDegree() const noexcept { return static_cast<T>(Math::ToDegree(Radian)); }

private:
	ImplType Radian;
};

template<typename V, typename T>
inline TAngle<T> operator*(V Factor, const TAngle<T>& Angle) noexcept { return Angle * Factor; }

// Angle operations on PI.
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
inline TAngle<T> operator*(Math::PI Pi, T Factor) noexcept { return Pi.Value * Factor; }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
inline TAngle<T> operator/(Math::PI Pi, T Divisor) noexcept { return operator*(Pi, std::divides<T>{}(1, Divisor)); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
inline decltype(auto) operator*(T Factor, Math::PI Pi) noexcept { return Pi * Factor; }

// Alias for typical use.
using FAngle = TAngle<float>;
