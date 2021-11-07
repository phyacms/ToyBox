// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "AxisIndex.h"

template<typename T, std::size_t N>
class TVector final
{
public:
	using ValueType = T;
	static_assert(std::is_floating_point_v<ValueType>);
	inline static constexpr auto Dimension{ N };

public:
	TVector() : Components{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
	explicit TVector(Parameters&&... Params) : Components{ static_cast<ValueType>(Params)... } {}
	TVector(const TVector&) = default;
	TVector& operator=(const TVector&) = default;
	TVector(TVector&&) noexcept = default;
	TVector& operator=(TVector&&) noexcept = default;
	~TVector() noexcept = default;

	friend inline bool operator==(const TVector& Lhs, const TVector& Rhs) noexcept = default;
	friend inline bool operator!=(const TVector& Lhs, const TVector& Rhs) noexcept = default;

	inline TVector operator+() const { return *this; }
	inline TVector operator-() const
	{
		TVector U{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Components),
			std::cend(Components),
			std::begin(U.Components),
			std::negate<ValueType>{});
		return U;
	}

	inline TVector operator+(const TVector& V) const { TVector U{ *this }; U += V; return U; }
	inline TVector operator-(const TVector& V) const { TVector U{ *this }; U -= V; return U; }
	inline TVector operator*(const ValueType& Factor) const { TVector U{ *this }; U *= Factor; return U; }
	inline TVector operator*(const TVector& Factors) const { TVector U{ *this }; U *= Factors; return U; }
	inline TVector operator/(const ValueType& Divisor) const { TVector U{ *this }; U /= Divisor; return U; }
	inline TVector operator/(const TVector& Divisors) const { TVector U{ *this }; U /= Divisors; return U; }
	friend inline TVector operator*(const ValueType& Factor, const TVector& V) { return V * Factor; }

	inline TVector& operator+=(const TVector& V)
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Components),
			std::cend(Components),
			std::cbegin(V.Components),
			std::begin(Components),
			std::plus<ValueType>{});
		return *this;
	}
	inline TVector& operator-=(const TVector& V) { return operator+=(-V); }
	inline TVector& operator*=(const ValueType& Factor)
	{
		TVector U{};
		std::fill(std::execution::par_unseq, std::begin(U.Components), std::end(U.Components), Factor);
		return operator*=(std::move(U));
	}
	inline TVector& operator*=(const TVector& Factors)
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Components),
			std::cend(Components),
			std::cbegin(Factors.Components),
			std::begin(Components),
			std::multiplies<ValueType>{});
		return *this;
	}
	inline TVector& operator/=(const ValueType& Divisor) { return operator*=(std::divides<ValueType>{}(ValueType{ 1 }, Divisor)); }
	inline TVector& operator/=(TVector Divisors)
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Divisors.Components),
			std::cend(Divisors.Components),
			std::begin(Divisors.Components),
			std::bind_front(std::divides<ValueType>{}, ValueType{ 1 }));
		return operator*=(std::move(Divisors));
	}

	inline ValueType& operator[](std::size_t Index) { return Components[Index]; }
	inline const ValueType& operator[](std::size_t Index) const { return Components[Index]; }

public:
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType& At() noexcept { return At(Index); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline const ValueType& At() const noexcept { return At(Index); }
	inline ValueType& At(std::size_t Index) { return Components.at(Index); }
	inline const ValueType& At(std::size_t Index) const { return Components.at(Index); }
	EnumerateAxisIndex(DeclareAxisIndexOperations)

	inline bool IsZero() const noexcept { return *this == TVector{}; }
	inline bool IsNormalized() const noexcept { return LengthSq() == ValueType{ 1 }; }
	inline ValueType LengthSq() const noexcept { return DotProduct(*this); }
	inline ValueType Length() const noexcept { return std::sqrt(LengthSq()); }
	inline TVector Normalized() const { TVector U{ *this }; U.Normalize(); return U; }
	inline ValueType DotProduct(const TVector& V) const { return operator*(V).Summation(); }
	template<typename RetType = std::enable_if_t<Dimension == 3, TVector>>
	inline TVector CrossProduct(const TVector& V) const
	{
		static constexpr auto NextIndexOf{ [](std::size_t Index)->std::size_t { return (Index + std::size_t{ 1 }) % Dimension; } };
		static constexpr auto PrevIndexOf{ [](std::size_t Index)->std::size_t { return (Index + (Dimension - std::size_t{ 1 })) % Dimension; } };

		TVector U{};
		for (std::size_t Index{}; Index != Dimension; ++Index)
		{
			U[Index]
				= At(NextIndexOf(Index)) * V.At(PrevIndexOf(Index))
				- At(PrevIndexOf(Index)) * V.At(NextIndexOf(Index));
		}
		return U;
	}
	inline ValueType Summation() const
	{
		return std::reduce(
			std::execution::par_unseq,
			std::cbegin(Components),
			std::cend(Components),
			ValueType{});
	}
	inline ValueType Multiplication() const
	{
		return std::reduce(
			std::execution::par_unseq,
			std::cbegin(Components),
			std::cend(Components),
			ValueType{ 1 },
			std::multiplies<ValueType>{});
	}

	inline TVector& Normalize() noexcept { return operator/=(Length()); }

private:
	std::array<T, N> Components;
};
