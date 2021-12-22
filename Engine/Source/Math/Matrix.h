// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Vector.h"

// Represents NxM matrix of type T.
// @NOTE: Elements are aligned in row-major ordering.
// e.g.) TMatrix<T, 3, 4> has elements: { _11, _12, _13, _14
//                                        _21, _22, _23, _24
//                                        _31, _32, _33, _34 }
template<typename T, std::size_t R, std::size_t C = R>
class TMatrix final
{
public:
	inline static constexpr auto RowDimension{ R };
	inline static constexpr auto ColumnDimension{ C };
	inline static constexpr auto bIsSquare{ RowDimension == ColumnDimension };

	using ValueType = T;
	using RowVectorType = TVector<ValueType, ColumnDimension>;
	using ColumnVectorType = TVector<ValueType, RowDimension>;

	static_assert(
		std::is_floating_point_v<ValueType>
		&& RowDimension > 0
		&& ColumnDimension > 0);

private:
	inline static constexpr auto ElementCount{ RowDimension * ColumnDimension };
	inline static constexpr decltype(auto) CalcIndex(std::size_t RowIndex, std::size_t ColumnIndex) noexcept { return RowIndex * ColumnDimension + ColumnIndex; }

public:
	inline static constexpr bool IsSquare() noexcept { return bIsSquare; }

	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline static TMatrix GetIdentity() noexcept
	{
		static constexpr auto Dimension{ RowDimension };

		TMatrix M{};
		for (std::size_t Index{}; Index != Dimension; ++Index)
		{
			M.At(Index, Index) = 1;
		}
		return M;
	}

public:
	TMatrix() : Elements{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == ElementCount>>
		explicit TMatrix(Parameters&&... Params) : Elements{ static_cast<ValueType>(Params)... } {}
	TMatrix(const TMatrix&) = default;
	TMatrix& operator=(const TMatrix&) & = default;
	TMatrix(TMatrix&&) noexcept = default;
	TMatrix& operator=(TMatrix&&) & noexcept = default;
	~TMatrix() noexcept { static_assert(sizeof(TMatrix) == sizeof(ValueType) * RowDimension * ColumnDimension); }

	inline TMatrix operator+() const noexcept { return *this; }
	inline TMatrix operator-() const noexcept
	{
		TMatrix M{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::begin(M),
			std::negate<ValueType>{});
		return M;
	}

	inline TMatrix operator+(const TMatrix& M) const noexcept { TMatrix N{ *this }; N += M; return N; }
	inline TMatrix operator-(const TMatrix& M) const noexcept { TMatrix N{ *this }; N -= M; return N; }
	inline TMatrix& operator+=(const TMatrix& M) & noexcept
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::cbegin(*this),
			std::begin(*this),
			std::plus<ValueType>{});
		return *this;
	}
	inline TMatrix& operator-=(const TMatrix& M) & noexcept
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::cbegin(*this),
			std::begin(*this),
			std::minus<ValueType>{});
		return *this;
	}

	inline TMatrix operator*(const ValueType& Factor) const noexcept { TMatrix N{ *this }; N *= Factor; return N; }
	inline TMatrix operator/(const ValueType& Divisor) const noexcept { TMatrix N{ *this }; N /= Divisor; return N; }
	inline TMatrix& operator*=(const ValueType& Factor) & noexcept
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(*this),
			std::cend(*this),
			std::begin(*this),
			std::bind_front(std::multiplies<ValueType>{}, Factor));
		return *this;
	}
	inline TMatrix& operator/=(const ValueType& Divisor) & noexcept { return operator*=(std::divides<ValueType>{}(1, Divisor)); }

	template<std::size_t Dimension>
	inline TMatrix<ValueType, RowDimension, Dimension> operator*(const TMatrix<ValueType, ColumnDimension, Dimension>& M) const noexcept
	{
		TMatrix<ValueType, RowDimension, Dimension> N{};

		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			for (std::size_t ColumnIndex{}; ColumnIndex != Dimension; ++ColumnIndex)
			{
				for (std::size_t Index{}; Index != ColumnDimension; ++Index)
				{
					N.At(RowIndex, ColumnIndex) += At(RowIndex, Index) * M.At(Index, ColumnIndex);
				}
			}
		}
		return N;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& operator*=(const TMatrix& SqM) & noexcept { return (*this = operator*(SqM));; }

	inline ColumnVectorType operator*(const RowVectorType& V) const noexcept
	{
		ColumnVectorType U{};
		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			for (std::size_t Index{}; Index != ColumnDimension; ++Index)
			{
				U[RowIndex] += At(RowIndex, Index) * V[Index];
			}
		}
		return U;
	}

public:
	inline ValueType& operator[](std::size_t Index) & { return Elements[Index]; }
	inline const ValueType& operator[](std::size_t Index) const& { return Elements[Index]; }
	inline ValueType operator[](std::size_t Index) const&& { return Elements[Index]; }

	inline ValueType& At(std::size_t Index) & { return Elements.at(Index); }
	inline const ValueType& At(std::size_t Index) const& { return Elements.at(Index); }
	inline ValueType At(std::size_t Index) const&& { return Elements.at(Index); }

	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline ValueType& At() & noexcept { return std::get<Index>(Elements); }
	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline const ValueType& At() const& noexcept { return std::get<Index>(Elements); }
	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline ValueType At() const&& noexcept { return std::get<Index>(Elements); }

	inline ValueType& At(std::size_t RowIndex, std::size_t ColumnIndex) & { return At(CalcIndex(RowIndex, ColumnIndex)); }
	inline const ValueType& At(std::size_t RowIndex, std::size_t ColumnIndex) const& { return At(CalcIndex(RowIndex, ColumnIndex)); }
	inline ValueType At(std::size_t RowIndex, std::size_t ColumnIndex) const&& { return At(CalcIndex(RowIndex, ColumnIndex)); }

	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline ValueType& At() & noexcept { return At<CalcIndex(RowIndex, ColumnIndex)>(); }
	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline const ValueType& At() const& noexcept { return At<CalcIndex(RowIndex, ColumnIndex)>(); }
	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline ValueType At() const&& noexcept { return At<CalcIndex(RowIndex, ColumnIndex)>(); }

	ExposeIterators(Elements)
	ExposeReverseIterators(Elements)

public:
	inline bool IsZero() const noexcept { return *this == TMatrix{}; }
	template<typename RetType = std::enable_if_t<IsSquare(), bool>>
	inline RetType IsIdentity() const noexcept { return *this == GetIdentity(); }
	template<typename RetType = std::enable_if_t<IsSquare(), bool>>
	inline RetType IsInvertable() const noexcept
	{
		const auto& Det{ Determinant() };
		return std::isnan(Det) || Math::IsEqualTo<ValueType>(Det, 0);
	}
	template<typename RetType = std::enable_if_t<IsSquare(), RowVectorType>>
	inline RowVectorType Diagonal() const noexcept
	{
		static constexpr auto Dimension{ RowDimension };
		using VectorType = RowVectorType;

		VectorType V{};
		for (std::size_t Index{}; Index != Dimension; ++Index)
		{
			V[Index] = At(Index, Index);
		}
		return V;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), ValueType>>
	inline RetType Determinant() const noexcept { return LUDecomposed().Diagonal().Multiplication(); }

	inline TMatrix<ValueType, ColumnDimension, RowDimension> Transposed() const
	{
		if constexpr (IsSquare())
		{
			TMatrix M{ *this };
			M.Transpose();
			return M;
		}
		else
		{
			TMatrix<ValueType, ColumnDimension, RowDimension> M{};
			for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
			{
				for (std::size_t ColumnIndex{}; ColumnIndex != ColumnDimension; ++ColumnIndex)
				{
					At(ColumnIndex, RowIndex) = At(RowIndex, ColumnIndex);
				}
			}
			return M;
		}
	}
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType Inverse() const
	{
		static constexpr auto Dimension{ RowDimension };
		using VectorType = RowVectorType;

		const auto& LU{ LUDecomposed() };
		const auto& Det{ LU.Diagonal().Multiplication() };
		if (std::isnan(Det) || Math::IsEqualTo<ValueType>(Det, 0))
		{
			throw std::runtime_error{ "Tried to get inverse matrix of singular matrix. (det M == 0)" };
		}

		// Solves M_this * Y = E to find the inverse matrix;
		TMatrix Inv{};
		for (std::size_t RowIndex{}; RowIndex < Dimension; RowIndex++)
		{
			VectorType E{};
			E[RowIndex] = 1;

			VectorType Y{};
			for (std::size_t ColumnIndex{}; ColumnIndex != Dimension; ++ColumnIndex)
			{
				ValueType X{};
				for (std::size_t Index{}; Index != ColumnIndex; ++Index)
				{
					X += LU.At(Index, ColumnIndex) * Y[Index];
				}
				Y[ColumnIndex] = (E[ColumnIndex] - X);
			}

			for (std::size_t ReverseColumnIndex = Dimension; ReverseColumnIndex != 0; --ReverseColumnIndex)
			{
				const auto ColumnIndex{ ReverseColumnIndex - 1 };

				ValueType X{};
				for (std::size_t Index{ ReverseColumnIndex }; Index != Dimension; ++Index)
				{
					X += LU.At(Index, ColumnIndex) * Inv.At(RowIndex, Index);
				}
				Inv.At(RowIndex, ColumnIndex) = (Y[ColumnIndex] - X) / LU.At(ColumnIndex, ColumnIndex);
			}
		}
		return Inv;
	}

	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& Transpose() noexcept
	{
		static constexpr auto Dimension{ RowDimension };

		std::size_t TaskIndex{};
		for (std::size_t RowIndex{}; RowIndex != Dimension - 1; ++RowIndex)
		{
			for (std::size_t ColumnIndex{ RowIndex + 1 }; ColumnIndex != ColumnDimension; ++ColumnIndex)
			{
				std::swap(At(ColumnIndex, RowIndex), At(RowIndex, ColumnIndex));
			}
		}
		return *this;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& Invert() { return *this = Inverse(); }

public:
	inline ValueType* GetPtr() noexcept { return Elements.data(); }
	inline const ValueType* GetPtr() const noexcept { return Elements.data(); }

private:
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType LUDecomposed() const noexcept
	{
		static constexpr auto Dimension{ RowDimension };

		RetType LU{};

		for (std::size_t RowIndex{}; RowIndex < Dimension; RowIndex++)
		{
			for (std::size_t ColumnIndex{}; ColumnIndex < Dimension; ColumnIndex++)
			{
				if (RowIndex <= ColumnIndex)
				{
					LU.At(ColumnIndex, RowIndex) = At(ColumnIndex, RowIndex);
					for (std::size_t Index{}; Index < RowIndex; Index++)
					{
						LU.At(ColumnIndex, RowIndex) -= LU.At(ColumnIndex, Index) * LU.At(Index, RowIndex);
					}
				}
			}

			for (std::size_t ColumnIndex{}; ColumnIndex < Dimension; ColumnIndex++)
			{
				if (RowIndex < ColumnIndex)
				{
					LU.At(RowIndex, ColumnIndex) = At(RowIndex, ColumnIndex) / LU.At(RowIndex, RowIndex);
					for (std::size_t Index{}; Index < RowIndex; Index++)
					{
						LU.At(RowIndex, ColumnIndex) -= (LU.At(RowIndex, Index) * LU.At(Index, ColumnIndex)) / LU.At(RowIndex, RowIndex);
					}
				}
			}
		}

		return LU;
	}

private:
	std::array<ValueType, ElementCount> Elements;
};

template<typename T, std::size_t N>
inline bool operator==(const TMatrix<T, N>& Lhs, const TMatrix<T, N>& Rhs) noexcept
{
	return std::equal(
		std::execution::par_unseq,
		std::cbegin(Lhs),
		std::cend(Lhs),
		std::cbegin(Rhs),
		Math::TIsEqualTo<T>{});
}

template<typename T, std::size_t N>
inline bool operator!=(const TMatrix<T, N>& Lhs, const TMatrix<T, N>& Rhs) noexcept
{
	return std::equal(
		std::execution::par_unseq,
		std::cbegin(Lhs),
		std::cend(Lhs),
		std::cbegin(Rhs),
		Math::TIsNotEqualTo<T>{});
}

template<typename T, std::size_t N>
inline TMatrix<T, N> operator*(const T& Factor, const TMatrix<T, N> M) noexcept { return M * Factor; }

template<typename T, std::size_t N>
inline TMatrix<T, N>::RowVectorType operator*(const typename TMatrix<T, N>::ColumnVectorType& V, const TMatrix<T, N>& M) noexcept { return M.Transposed() * V; }
