// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "AxisIndex.h"
#include "Vector.h"

template<typename T, std::size_t N>
class TPoint final
{
public:
	using CompatibleVectorType = TVector<T, N>;
	using ValueType = CompatibleVectorType::ValueType;
	inline static constexpr auto Dimension{ CompatibleVectorType::Dimension };

public:
	TPoint() : Coord{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
	explicit TPoint(Parameters&&... Params) : Coord{ std::forward<Parameters>(Params)... } {}
	explicit TPoint(const CompatibleVectorType& V) : Coord{ V } {}
	TPoint(const TPoint&) = default;
	TPoint& operator=(const TPoint&) = default;
	TPoint(TPoint&&) noexcept = default;
	TPoint& operator=(TPoint&&) noexcept = default;
	~TPoint() noexcept = default;

	friend inline bool operator==(const TPoint& Lhs, const TPoint& Rhs) noexcept = default;
	friend inline bool operator!=(const TPoint& Lhs, const TPoint& Rhs) noexcept = default;

	inline TPoint operator+() const { return *this; }
	inline TPoint operator-() const { return TPoint{ -Coord }; }

	inline TPoint operator+(const CompatibleVectorType& V) const { TPoint P{ *this }; P += V; return P; }
	inline TPoint operator-(const CompatibleVectorType& V) const { TPoint P{ *this }; P -= V; return P; }
	inline CompatibleVectorType operator-(const TPoint& P) const { return Coord - P.Coord; }
	inline TPoint operator*(const ValueType& Factor) const { TPoint P{ *this }; P *= Factor; return P; }
	inline TPoint operator*(const CompatibleVectorType& Factors) const { TPoint P{ *this }; P *= Factors; return P; }
	inline TPoint operator/(const ValueType& Divisor) const { TPoint P{ *this }; P /= Divisor; return P; }
	inline TPoint operator/(const CompatibleVectorType& Divisors) const { TPoint P{ *this }; P /= Divisors; return P; }
	friend inline TPoint operator+(const CompatibleVectorType& V, const TPoint& P) { return P + V; }
	friend inline TPoint operator*(const ValueType& Factor, const TPoint& P) { return Factor * P; }

	inline TPoint& operator+=(const CompatibleVectorType& V) { Coord += V; return *this; }
	inline TPoint& operator-=(const CompatibleVectorType& V) { Coord -= V; return *this; }
	inline TPoint& operator*=(const ValueType& Factor) { Coord *= Factor; return *this; }
	inline TPoint& operator*=(const CompatibleVectorType& Factors) { Coord *= Factors; return *this; }
	inline TPoint& operator/=(const ValueType& Divisor) { Coord /= Divisor; return *this; }
	inline TPoint& operator/=(const CompatibleVectorType& Divisors) { Coord /= Divisors; return *this; }

	inline ValueType& operator[](std::size_t Index) { return Coord.operator[](Index); }
	inline const ValueType& operator[](std::size_t Index) const { return Coord.operator[](Index); }

public:
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType& At() noexcept { return At(Index); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline const ValueType& At() const noexcept { return At(Index); }
	inline ValueType& At(std::size_t Index) { return Coord.At(Index); }
	inline const ValueType& At(std::size_t Index) const { return Coord.At(Index); }
	EnumerateAxisIndex(DeclareAxisIndexOperations)

	inline bool IsOrigin() const noexcept { return Coord.IsZero(); }
	inline ValueType DistanceSq(const TPoint& P = TPoint{}) const { return operator-(P).LengthSquare(); }
	inline ValueType Distance(const TPoint& P = TPoint{}) const { return operator-(P).Length(); }
	inline const CompatibleVectorType& FromOrigin() const noexcept { return Coord; }

private:
	CompatibleVectorType Coord;
};
