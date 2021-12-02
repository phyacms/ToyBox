// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "ScreenLocation.h"
#include "ScreenSize.h"

class URelCoord;

class UAbsCoord final
{
	friend UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept;
	friend class UCoord;

public:
	using VectorType = TVector<float, 2>;

public:
	UAbsCoord(const FScreenLocation& Coord = {});
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == VectorType::Dimension>>
		explicit UAbsCoord(Parameters&&... Params) : Coord{ static_cast<VectorType::ValueType>(Params)... } {}
	UAbsCoord(const UAbsCoord&) = default;
	UAbsCoord& operator=(const UAbsCoord&) & = default;
	UAbsCoord(UAbsCoord&&) noexcept = default;
	UAbsCoord& operator=(UAbsCoord&&) & noexcept = default;
	~UAbsCoord() noexcept = default;

public:
	inline const VectorType& GetAsVector() const noexcept { return Coord; }
	inline const float* GetPtr() const noexcept { return Coord.GetPtr(); }

private:
	VectorType Coord;
};

UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept;
