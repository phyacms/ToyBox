// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Math/AxisIndex.h"
#include "Math/Vector.h"

template<typename T, std::size_t N>
class TNormals final
{
public:
	using VectorType = TVector<T, N>;

	inline static constexpr auto Dimension{ VectorType::Dimension };

	using ValueType = VectorType::ValueType;

public:
	TNormals() : Norms{} {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
		explicit TNormals(Parameters&&... Params) : Norms{ static_cast<ValueType>(Params)... } { Clamp(); }
	TNormals(const TNormals&) = default;
	TNormals& operator=(const TNormals&) & = default;
	TNormals(TNormals&&) noexcept = default;
	TNormals& operator=(TNormals&&) & noexcept = default;
	~TNormals() noexcept { static_assert(sizeof(TNormals) == sizeof(ValueType) * Dimension); }

	friend inline bool operator==(const TNormals& Lhs, const TNormals& Rhs) noexcept = default;
	friend inline bool operator!=(const TNormals& Lhs, const TNormals& Rhs) noexcept = default;

public:
	inline const ValueType& operator[](std::size_t Index) const& { return Norms[Index]; }
	inline ValueType operator[](std::size_t Index) const&& { return Norms[Index]; }

	inline const ValueType& At(std::size_t Index) const& { return Norms.At(Index); }
	inline ValueType At(std::size_t Index) const&& { return Norms.At(Index); }

	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline const ValueType& At() const& noexcept { return std::get<Index>(Norms); }
	template<std::size_t Index, typename = std::enable_if_t<Index < Dimension>>
	inline ValueType At() const&& noexcept { return std::get<Index>(Norms); }

	EnumerateAxisIndex(DeclareAxisIndexReadOperations)
	EnumerateConstIterators(Norms, ExposeConstIterators)
	EnumerateConstReverseIterators(Norms, ExposeConstIterators)

public:
	template<typename T>
	inline T MapAt(std::size_t Index, const T& Min = 0, const T& Max = 1) const
	{
		return static_cast<T>(std::lerp(
			static_cast<ValueType>(Min),
			static_cast<ValueType>(Max),
			At(Index)));
	}
	template<
		typename T,
		std::size_t Index,
		T Min = 0,
		T Max = 1,
		typename = std::enable_if_t<Index < Dimension>>
	inline ValueType MapAt() const noexcept { return MapAt<T>(Index, Min, Max); }

	template<typename T>
	inline TVector<T, Dimension> Map(const T& Min = 0, const T& Max = 1) const noexcept
	{
		TVector<T, Dimension> Mapped{};
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Norms),
			std::cend(Norms),
			std::begin(Mapped),
			[&Min, &Max](const ValueType& Value)->T {
				return static_cast<T>(std::lerp(
					static_cast<ValueType>(Min),
					static_cast<ValueType>(Max),
					Value)); });
		return Mapped;
	};

	template<typename T>
	inline void SetAt(std::size_t Index, const T& Value, const T& Min = 0, const T& Max = 1)
	{
		Norms[Index] = Min != Max
			? static_cast<ValueType>(Value - Min) / static_cast<ValueType>(Max - Min)
			: static_cast<ValueType>(Min);
	}
	template<typename T, typename C>
	inline void Set(const C& Container, const T& Min = 0, const T& Max = 1)
	{
		std::transform(
			std::execution::par_unseq,
			std::cbegin(Container),
			std::cend(Container),
			std::begin(Norms),
			[&Min, &Max](const T& Value)->ValueType {
				return Min != Max
					? std::lerp(
						0.0f,
						1.0f,
						static_cast<float>(Value - Min) / (static_cast<float>(Max - Min)))
					: static_cast<float>(Min); });
	}

public:
	inline const VectorType& GetAsVector() const noexcept { return Norms; }

	inline ValueType* GetPtr() noexcept { return Norms.data(); }
	inline const ValueType* GetPtr() const noexcept { return Norms.GetPtr(); }

private:
	inline void Clamp() noexcept
	{
		std::for_each(
			std::execution::par_unseq,
			std::begin(Norms),
			std::end(Norms),
			[](ValueType& Value)->void { Value = std::clamp<ValueType>(Value, 0, 1); });
	}

private:
	VectorType Norms;
};
