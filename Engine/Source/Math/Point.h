// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "AxisIndex.h"
#include "Vector.h"

template<typename T, std::size_t N>
class TPoint final
{
public:
	inline static constexpr auto Dimension{ N };
	using ValueType = T;
	using DiffType = TVector<ValueType, Dimension>;

public:
	TPoint() : Coord{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
	explicit TPoint(Parameters&&... Params) : Coord{ std::forward<Parameters>(Params)... } {}
	explicit TPoint(const DiffType& V) : Coord{ V } {}
	TPoint& operator=(const DiffType& V) & { Coord = V; return *this; }
	explicit TPoint(DiffType&& V) noexcept : Coord{ std::move(V) } {}
	TPoint& operator=(DiffType&& V) & noexcept { Coord = std::move(V); return *this; }
	TPoint(const TPoint&) = default;
	TPoint& operator=(const TPoint&) & = default;
	TPoint(TPoint&&) noexcept = default;
	TPoint& operator=(TPoint&&) & noexcept = default;
	~TPoint() noexcept { static_assert(sizeof(TPoint) == sizeof(ValueType) * Dimension); }

	friend inline bool operator==(const TPoint& Lhs, const TPoint& Rhs) noexcept = default;
	friend inline bool operator!=(const TPoint& Lhs, const TPoint& Rhs) noexcept = default;

	inline const TPoint& operator+() const& { return *this; }
	inline TPoint operator+() const&& { return *this; }
	inline TPoint operator-() const { return TPoint{ -Coord }; }

	inline TPoint operator+(const DiffType& V) const { TPoint P{ *this }; P += V; return P; }
	inline TPoint operator-(const DiffType& V) const { TPoint P{ *this }; P -= V; return P; }
	inline DiffType operator-(const TPoint& P) const { return Coord - P.Coord; }
	inline TPoint operator*(const ValueType& Factor) const { TPoint P{ *this }; P *= Factor; return P; }
	inline TPoint operator*(const DiffType& Factors) const { TPoint P{ *this }; P *= Factors; return P; }
	inline TPoint operator/(const ValueType& Divisor) const { TPoint P{ *this }; P /= Divisor; return P; }
	inline TPoint operator/(const DiffType& Divisors) const { TPoint P{ *this }; P /= Divisors; return P; }

	inline TPoint& operator+=(const DiffType& V) & { Coord += V; return *this; }
	inline TPoint& operator-=(const DiffType& V) & { Coord -= V; return *this; }
	inline TPoint& operator*=(const ValueType& Factor) & { Coord *= Factor; return *this; }
	inline TPoint& operator*=(const DiffType& Factors) & { Coord *= Factors; return *this; }
	inline TPoint& operator/=(const ValueType& Divisor) & { Coord /= Divisor; return *this; }
	inline TPoint& operator/=(const DiffType& Divisors) & { Coord /= Divisors; return *this; }

public:
	inline ValueType& operator[](std::size_t Index) & { return Coord[Index]; }
	inline const ValueType& operator[](std::size_t Index) const& { return Coord[Index]; }
	inline ValueType operator[](std::size_t Index) const&& { return Coord[Index]; }

	inline ValueType& At(std::size_t Index) & { return Coord.At(Index); }
	inline const ValueType& At(std::size_t Index) const& { return Coord.At(Index); }
	inline ValueType At(std::size_t Index) const&& { return Coord.At(Index); }

	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType& At() & noexcept { return Coord.At<Index>(); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline const ValueType& At() const& noexcept { return Coord.At<Index>(); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType At() const&& noexcept { return Coord.At<Index>(); }

	EnumerateAxisIndex(DeclareAxisIndexOperations)
	DeclareIterators(Coord)
	DeclareReverseIterators(Coord)

public:
	inline bool IsOrigin() const noexcept { return Coord.IsZero(); }
	inline ValueType DistanceSq(const TPoint& P = TPoint{}) const { return operator-(P).LengthSq(); }
	template<typename T = DiffType::LengthType>
	inline T Distance(const TPoint& P = TPoint{}) const { return operator-(P).Length<T>(); }
	inline const DiffType& FromOrigin() const noexcept { return Coord; }

public:
	inline ValueType* GetPtr() noexcept { return Coord.GetPtr(); }
	inline const ValueType* GetPtr() const noexcept { return Coord.GetPtr(); }

private:
	DiffType Coord;
};
