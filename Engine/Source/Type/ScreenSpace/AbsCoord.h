// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "ScreenLocation.h"
#include "ScreenSize.h"

using UVector = TVector<float, 2>;
class URelCoord;

class UAbsCoord final
{
	friend UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept;
	friend class UCoord;

public:
	UAbsCoord(const FScreenLocation& Coord = {});
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == UVector::Dimension>>
		explicit UAbsCoord(Parameters&&... Params) : Coord{ static_cast<UVector::ValueType>(Params)... } {}
	UAbsCoord(const UAbsCoord&) = default;
	UAbsCoord& operator=(const UAbsCoord&) & = default;
	UAbsCoord(UAbsCoord&&) noexcept = default;
	UAbsCoord& operator=(UAbsCoord&&) & noexcept = default;
	~UAbsCoord() noexcept = default;

public:
	inline const UVector& GetAsVector() const noexcept { return Coord; }
	inline const float* GetPtr() const noexcept { return Coord.GetPtr(); }

private:
	UVector Coord;
};

UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept;
