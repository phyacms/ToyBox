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

		std::array<std::future<void>, Dimension> Tasks{};
		const auto Set{ [&M](std::size_t Index)->void { M.At(Index, Index) = 1; } };

		for (std::size_t Index{}; Index != Dimension; ++Index)
		{
			Tasks[Index] = std::async(Set, Index);
		}
		for (auto& Task : Tasks) { Task.get(); }

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

	friend inline bool operator==(const TMatrix& Lhs, const TMatrix& Rhs) noexcept
	{
		return std::equal(
			std::execution::par_unseq,
			std::cbegin(Lhs.Elements),
			std::cend(Lhs.Elements),
			std::cbegin(Rhs.Elements),
			MathFunctions::TIsEqualTo<ValueType>{});
	}
	friend inline bool operator!=(const TMatrix& Lhs, const TMatrix& Rhs) noexcept
	{
		return std::equal(
			std::execution::par_unseq,
			std::cbegin(Lhs.Elements),
			std::cend(Lhs.Elements),
			std::cbegin(Rhs.Elements),
			MathFunctions::TIsNotEqualTo<ValueType>{});
	}

	inline TMatrix operator+() const { return *this; }
	inline TMatrix operator-() const
	{
		TMatrix M{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Elements),
			std::cend(Elements),
			std::begin(M.Elements),
			std::negate<ValueType>{});
		return M;
	}

	inline TMatrix operator+(const TMatrix& M) const { TMatrix N{ *this }; N += M; return N; }
	inline TMatrix operator-(const TMatrix& M) const { TMatrix N{ *this }; N -= M; return N; }
	inline TMatrix& operator+=(const TMatrix& M) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Elements),
			std::cend(Elements),
			std::cbegin(Elements),
			std::begin(Elements),
			std::plus<ValueType>{});
		return *this;
	}
	inline TMatrix& operator-=(const TMatrix& M) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Elements),
			std::cend(Elements),
			std::cbegin(Elements),
			std::begin(Elements),
			std::minus<ValueType>{});
		return *this;
	}

	inline TMatrix operator*(const ValueType& Factor) const { TMatrix N{ *this }; N *= Factor; return N; }
	inline TMatrix operator/(const ValueType& Divisor) const { TMatrix N{ *this }; N /= Divisor; return N; }
	inline TMatrix& operator*=(const ValueType& Factor) &
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Elements),
			std::cend(Elements),
			std::begin(Elements),
			std::bind_front(std::multiplies<ValueType>{}, Factor));
		return *this;
	}
	inline TMatrix& operator/=(const ValueType& Divisor) & { return operator*=(std::divides<ValueType>{}(1, Divisor)); }
	friend inline TMatrix operator*(const ValueType& Factor, const TMatrix M) { return M * Factor; }

	template<std::size_t Dimension>
	inline TMatrix<ValueType, RowDimension, Dimension> operator*(const TMatrix<ValueType, ColumnDimension, Dimension>& M) const
	{
		TMatrix<ValueType, RowDimension, Dimension> N{};
		const auto& Rows{ GetRows() };
		const auto& Columns{ M.GetColumns() };

		std::array<std::future<void>, ElementCount> Tasks{};
		const auto DotProduct{
			[&N, &Rows, &Columns](std::size_t RowIndex, std::size_t ColumnIndex)->void {
				N.At(RowIndex, ColumnIndex) = Rows[RowIndex].DotProduct(Columns[ColumnIndex]); } };

		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			for (std::size_t ColumnIndex{}; ColumnIndex != Dimension; ++ColumnIndex)
			{
				Tasks[CalcIndex(RowIndex, ColumnIndex)] = std::async(DotProduct, RowIndex, ColumnIndex);
			}
		}
		for (auto& Task : Tasks) { Task.get(); }

		return N;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& operator*=(const TMatrix& SqM) & { return (*this = operator*(SqM));; }

	inline ColumnVectorType operator*(const RowVectorType& V) const
	{
		ColumnVectorType U{};
		const auto& Rows{ GetRows() };

		std::array<std::future<void>, RowDimension> Tasks{};
		const auto DotProduct{
			[&U, &V, &Rows](std::size_t RowIndex)->void {
				U[RowIndex] = Rows[RowIndex].DotProduct(V); } };

		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			Tasks[RowIndex] = std::async(DotProduct, RowIndex);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return U;
	}
	friend inline RowVectorType operator*(const ColumnVectorType& V, const TMatrix& M) { return Transposed() * V; }

public:
	inline ValueType& operator[](std::size_t Index) & { return Elements[Index]; }
	inline const ValueType& operator[](std::size_t Index) const& { return Elements[Index]; }
	inline ValueType operator[](std::size_t Index) const&& { return Elements[Index]; }

	inline ValueType& At(std::size_t Index) & { return Elements.at(Index); }
	inline const ValueType& At(std::size_t Index) const& { return Elements.at(Index); }
	inline ValueType At(std::size_t Index) const&& { return Elements.at(Index); }

	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline ValueType& At() & { return std::get<Index>(Elements); }
	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline const ValueType& At() const& { return std::get<Index>(Elements); }
	template<std::size_t Index, typename = std::enable_if_t<Index < ElementCount>>
	inline ValueType At() const&& { return std::get<Index>(Elements); }

	inline ValueType& At(std::size_t RowIndex, std::size_t ColumnIndex) & { return At(CalcIndex(RowIndex, ColumnIndex)); }
	inline const ValueType& At(std::size_t RowIndex, std::size_t ColumnIndex) const& { return At(CalcIndex(RowIndex, ColumnIndex)); }
	inline ValueType At(std::size_t RowIndex, std::size_t ColumnIndex) const&& { return At(CalcIndex(RowIndex, ColumnIndex)); }

	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline ValueType& At() & { return At<CalcIndex(RowIndex, ColumnIndex)>(); }
	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline const ValueType& At() const& { return At<CalcIndex(RowIndex, ColumnIndex)>(); }
	template<
		std::size_t RowIndex,
		std::size_t ColumnIndex,
		typename = std::enable_if_t<CalcIndex(RowIndex, ColumnIndex) < ElementCount>>
	inline ValueType At() const&& { return At<CalcIndex(RowIndex, ColumnIndex)>(); }

	inline RowVectorType GetRow(std::size_t RowIndex) const noexcept
	{
		RowVectorType V{};

		std::array<std::future<void>, ColumnDimension> Tasks{};
		const auto Get{
			[this, &V, RowIndex](std::size_t ColumnIndex)->void {
				V[ColumnIndex] = At(RowIndex, ColumnIndex); } };

		for (std::size_t ColumnIndex{}; ColumnIndex != ColumnDimension; ++ColumnIndex)
		{
			Tasks[ColumnIndex] = std::async(Get, ColumnIndex);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return V;
	}
	inline ColumnVectorType GetColumn(std::size_t ColumnIndex) const noexcept
	{
		ColumnVectorType V{};

		std::array<std::future<void>, RowDimension> Tasks{};
		const auto Get{
			[this, &V, ColumnIndex](std::size_t RowIndex)->void {
				V[RowIndex] = At(RowIndex, ColumnIndex); } };

		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			Tasks[RowIndex] = std::async(Get, RowIndex);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return V;
	}

	inline std::array<RowVectorType, RowDimension> GetRows() const noexcept
	{
		std::array<RowVectorType, RowDimension> Rows{};

		std::array<std::future<void>, RowDimension> Tasks{};
		const auto Get{
			[this, &Rows](std::size_t RowIndex)->void {
				Rows[RowIndex] = GetRow(RowIndex); } };

		for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
		{
			Tasks[RowIndex] = std::async(Get, RowIndex);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return Rows;
	}
	inline std::array<ColumnVectorType, ColumnDimension> GetColumns() const noexcept
	{
		std::array<ColumnVectorType, ColumnDimension> Columns{};

		std::array<std::future<void>, ColumnDimension> Tasks{};
		const auto Get{
			[this, &Columns](std::size_t ColumnIndex)->void {
				Columns[ColumnIndex] = GetColumn(ColumnIndex); } };

		for (std::size_t ColumnIndex{}; ColumnIndex != ColumnDimension; ++ColumnIndex)
		{
			Tasks[ColumnIndex] = std::async(Get, ColumnIndex);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return Columns;
	}

public:
	inline bool IsZero() const noexcept { return *this == TMatrix{}; }
	template<typename RetType = std::enable_if_t<IsSquare(), bool>>
	inline RetType IsIdentity() const noexcept { return *this == GetIdentity(); }
	template<typename RetType = std::enable_if_t<IsSquare(), bool>>
	inline RetType IsInvertable() const noexcept
	{
		const auto& Det{ CalcDeterminant() };
		return std::isnan(Det) || MathFunctions::IsEqualTo<ValueType>(Det, 0);
	}
	template<typename RetType = std::enable_if_t<IsSquare(), RowVectorType>>
	inline RowVectorType GetDiagonal() const noexcept
	{
		static constexpr auto Dimension{ RowDimension };
		using VectorType = RowVectorType;
		RowVectorType V{};

		std::array<std::future<void>, Dimension> Tasks{};
		const auto Get{ [this, &V](std::size_t Index)->void { V[Index] = At(Index, Index); } };

		for (std::size_t Index{}; Index != Dimension; ++Index)
		{
			Tasks[Index] = std::async(Get, Index);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return V;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), ValueType>>
	inline RetType CalcDeterminant() const noexcept { return LUDecomposed().GetDiagonal().Multiplication(); }

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

			std::array<std::array<std::future<void>, ColumnDimension>, RowDimension> Tasks{};
			const auto Set{
				[this, &M](std::size_t RowIndex, std::size_t ColumnIndex)->void {
					At(ColumnIndex, RowIndex) = At(RowIndex, ColumnIndex); } };

			for (std::size_t RowIndex{}; RowIndex != RowDimension; ++RowIndex)
			{
				for (std::size_t ColumnIndex{}; ColumnIndex != ColumnDimension; ++ColumnIndex)
				{
					Tasks[RowIndex][ColumnIndex] = std::async(Set, RowIndex, ColumnIndex);
				}
			}
			for (auto& RowTasks : Tasks)
			{
				for (auto& Task : RowTasks)
				{
					Task.get();
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

		TMatrix Inv{};
		const auto& LU{ LUDecomposed() };
		const auto& Det{ LU.GetDiagonal().Multiplication() };
		if (std::isnan(Det) || MathFunctions::IsEqualTo<ValueType>(Det, 0))
		{
			throw std::runtime_error{ "Tried to get inverse matrix of singular matrix. (det M == 0)" };
		}

		// Solves M_this * Y = E to find the inverse matrix;
		std::array<std::future<void>, Dimension> Tasks{};
		const auto FindInvRow{
			[&Inv, &LU](std::size_t RowIndex)->void
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
			} };

		for (std::size_t Index{}; Index < Dimension; Index++)
		{
			Tasks[Index] = std::async(FindInvRow, Index);
		}
		for (auto& Task : Tasks) { Task.get(); }

		return Inv;
	}

	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& Transpose() noexcept
	{
		static constexpr auto Dimension{ RowDimension };

		std::array<std::future<void>, Dimension * (Dimension - 1) / 2> Tasks{};
		const auto Swap{
			[this](std::size_t RowIndex, std::size_t ColumnIndex)->void {
				std::swap(At(ColumnIndex, RowIndex), At(RowIndex, ColumnIndex)); } };

		std::size_t TaskIndex{};
		for (std::size_t RowIndex{}; RowIndex != Dimension - 1; ++RowIndex)
		{
			for (std::size_t ColumnIndex{ RowIndex + 1 }; ColumnIndex != ColumnDimension; ++ColumnIndex)
			{
				Tasks[TaskIndex++] = std::async(Swap, RowIndex, ColumnIndex);
			}
		}
		for (auto& Task : Tasks) { Task.get(); }

		return *this;
	}
	template<typename RetType = std::enable_if_t<IsSquare(), TMatrix>>
	inline RetType& Invert() { return *this = Inverse(); }

public:
	const ValueType* GetPtr() const noexcept { return Elements.data(); }

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
