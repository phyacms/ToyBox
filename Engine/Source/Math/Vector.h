// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Functions.h"
#include "AxisIndex.h"
#include "Type/Container.h"

template<typename T, std::size_t N>
class TVector final
{
public:
	inline static constexpr auto Dimension{ N };
	using ValueType = T;
	using LengthType = std::conditional_t<
		std::is_floating_point_v<ValueType>,
		ValueType,
		double>;
	static_assert(std::is_arithmetic_v<ValueType> && Dimension != 0);

public:
	TVector() : Components{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
	explicit TVector(Parameters&&... Params) : Components{ static_cast<ValueType>(Params)... } {}
	TVector(const TVector&) = default;
	TVector& operator=(const TVector&) & = default;
	TVector(TVector&&) noexcept = default;
	TVector& operator=(TVector&&) & noexcept = default;
	~TVector() noexcept { static_assert(sizeof(TVector) == sizeof(ValueType) * Dimension); }

	inline TVector operator+() const { return *this; }
	inline TVector operator-() const
	{
		TVector U{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::begin(U),
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

	inline TVector& operator+=(const TVector& V) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::cbegin(V),
			std::begin(*this),
			std::plus<ValueType>{});
		return *this;
	}
	inline TVector& operator-=(const TVector& V) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::cbegin(V),
			std::begin(*this),
			std::minus<ValueType>{});
		return *this;
	}
	inline TVector& operator*=(const ValueType& Factor) &
	{
		TVector U{};
		std::fill(
			std::execution::par_unseq,
			std::begin(U),
			std::end(U),
			Factor);
		return operator*=(std::move(U));
	}
	inline TVector& operator*=(const TVector& Factors) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::cbegin(Factors),
			std::begin(*this),
			std::multiplies<ValueType>{});
		return *this;
	}
	inline TVector& operator/=(const ValueType& Divisor) & { return operator*=(std::divides<ValueType>{}(1, Divisor)); }
	inline TVector& operator/=(TVector Divisors) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Divisors),
			std::cend(Divisors),
			std::begin(Divisors),
			std::bind_front(std::divides<ValueType>{}, ValueType{ 1 }));
		return operator*=(std::move(Divisors));
	}

public:
	inline ValueType& operator[](std::size_t Index) & { return Components[Index]; }
	inline const ValueType& operator[](std::size_t Index) const& { return Components[Index]; }
	inline ValueType operator[](std::size_t Index) const&& { return Components[Index]; }

	inline ValueType& At(std::size_t Index) & { return Components.at(Index); }
	inline const ValueType& At(std::size_t Index) const& { return Components.at(Index); }
	inline ValueType At(std::size_t Index) const&& { return Components.at(Index); }

	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType& At() & noexcept { return std::get<Index>(Components); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline const ValueType& At() const& noexcept { return std::get<Index>(Components); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType At() const&& noexcept { return std::get<Index>(Components); }

	EnumerateAxisIndex(DeclareAxisIndexOperations)
	ExposeIterators(Components)
	ExposeReverseIterators(Components)

public:
	inline bool IsZero() const noexcept { return *this == TVector{}; }
	inline bool IsUnit() const noexcept { return LengthSq() == ValueType{ 1 }; }
	inline bool IsParallelTo(const TVector& V) const noexcept { return Math::IsEqualTo<ValueType>(std::abs(DotProduct(V)), Length() * V.Length()); }
	inline bool IsPerpendicularTo(const TVector& V) const noexcept { return Math::IsEqualTo<ValueType>(DotProduct(V), 0); }
	inline ValueType LengthSq() const noexcept { return DotProduct(*this); }
	template<typename Type = LengthType>
	inline Type Length() const noexcept { return static_cast<Type>(std::sqrt(static_cast<LengthType>(LengthSq()))); }
	inline TVector Normalized() const { TVector U{ *this }; U.Normalize(); return U; }
	inline ValueType DotProduct(const TVector& V) const { return operator*(V).Summation(); }
	template<typename RetType = std::enable_if_t<Dimension == 3, TVector>>
	inline TVector CrossProduct(const TVector& V) const
	{
		static constexpr auto NextIndexOf{ [](std::size_t Index)->std::size_t { return (Index + 1) % Dimension; } };
		static constexpr auto PrevIndexOf{ [](std::size_t Index)->std::size_t { return (Index + (Dimension - 1)) % Dimension; } };

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
			std::cbegin(*this),
			std::cend(*this),
			ValueType{});
	}
	inline ValueType Multiplication() const
	{
		return std::reduce(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			ValueType{ 1 },
			std::multiplies<ValueType>{});
	}

	inline TVector& Normalize() noexcept { return operator/=(Length()); }

public:
	template<typename Type>
	inline TVector<Type, Dimension> CastAs() const noexcept
	{
		TVector<Type, Dimension> V{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::begin(V),
			[](const ValueType& Value)->Type { return static_cast<Type>(Value); });
		return V;
	}

	inline ValueType* GetPtr() noexcept { return Components.data(); }
	inline const ValueType* GetPtr() const noexcept { return Components.data(); }

private:
	std::array<ValueType, Dimension> Components;
};

template<typename T, std::size_t N>
inline bool operator==(const TVector<T, N>& Lhs, const TVector<T, N>& Rhs) noexcept
{
	using IsEqualTo = std::conditional_t<
		std::is_floating_point_v<T>,
		Math::TIsEqualTo<T>,
		std::equal_to<T>>;

	return std::equal(
		std::execution::par_unseq,
		std::cbegin(Lhs),
		std::cend(Lhs),
		std::cbegin(Rhs),
		IsEqualTo{});
}

template<typename T, std::size_t N>
inline bool operator!=(const TVector<T, N>& Lhs, const TVector<T, N>& Rhs) noexcept
{
	using IsNotEqualTo = std::conditional_t<
		std::is_floating_point_v<T>,
		Math::TIsNotEqualTo<T>,
		std::not_equal_to<T>>;

	return std::equal(
		std::execution::par_unseq,
		std::cbegin(Lhs),
		std::cend(Lhs),
		std::cbegin(Rhs),
		IsNotEqualTo{});
}
