// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Math/Normals.h"
#include "AbsCoord.h"

using UVector = TVector<float, 2>;
class UAbsCoord;

class URelCoord final
{
	friend URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept;
	friend class UCoord;

private:
	using UNorms = TNormals<float, 2>;
	static_assert(std::is_same_v<UNorms::VectorType, UVector>);

public:
	URelCoord(UNorms Coord = {}) : Norms{ std::move(Coord) } {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == UVector::Dimension>>
		explicit URelCoord(Parameters&&... Params) : Norms{ static_cast<UVector::ValueType>(Params)... } {}
	URelCoord(const URelCoord&) = default;
	URelCoord& operator=(const URelCoord&) & = default;
	URelCoord(URelCoord&&) noexcept = default;
	URelCoord& operator=(URelCoord&&) & noexcept = default;
	~URelCoord() noexcept = default;

public:
	inline const UVector& GetAsVector() const noexcept { return Norms.GetAsVector(); }
	inline const float* GetPtr() const noexcept { return Norms.GetPtr(); }

private:
	UNorms Norms;
};

URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept;
