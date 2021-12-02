// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Constants.h"

namespace Math
{
	// Equality comparison
	template<
		typename T,
		typename = std::enable_if_t<std::is_floating_point_v<std::remove_cvref_t<T>>>>
	class TIsEqualTo
	{
	public:
		TIsEqualTo(T ErrorLimit = T())
			: ErrorLimit{}
		{
			if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
			{
				constexpr T MinimumErrorLimit{ 1e-6f };
				if (ErrorLimit < MinimumErrorLimit) { ErrorLimit = MinimumErrorLimit; }
			}
			else if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
			{
				constexpr T MinimumErrorLimit{ 1e-15 };
				if (ErrorLimit < MinimumErrorLimit) { ErrorLimit = MinimumErrorLimit; }
			}
			this->ErrorLimit = ErrorLimit;
		}
		inline bool operator()(const T& Lhs, const T& Rhs) const noexcept { return std::abs(Lhs - Rhs) <= ErrorLimit; }

	private:
		T ErrorLimit{};
	};

	template<typename T>
	class TIsNotEqualTo final
		: private TIsEqualTo<T>
	{
	public:
		TIsNotEqualTo(T ErrorLimit = T()) : TIsEqualTo<T>(ErrorLimit) {}
		inline bool operator()(const T& Lhs, const T& Rhs) const noexcept { return !TIsEqualTo<T>::operator()(Lhs, Rhs); }
	};

	template<typename T>
	inline bool IsEqualTo(const T& Lhs, const T& Rhs, T ErrorLimit = T()) noexcept { return TIsEqualTo<T>{ ErrorLimit }(Lhs, Rhs); }

	template<typename T>
	inline bool IsNotEqualTo(const T& Lhs, const T& Rhs) noexcept { return TIsNotEqualTo<T>(Lhs, Rhs); }

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
