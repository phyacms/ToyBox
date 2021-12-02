// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "AbsCoord.h"
#include "RelCoord.h"

UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept
{
	UAbsCoord Abs{};
	Abs.Coord = Rel.GetAsVector() * Size.CastAs<float>();
	return Abs;
}

UAbsCoord::UAbsCoord(const FScreenLocation& Coord)
	: Coord{}
{
	std::transform(
		std::execution::par_unseq,
		std::cbegin(Coord),
		std::cend(Coord),
		std::begin(this->Coord),
		[](const FScreenLocation::ValueType& Value)->UVector::ValueType {
			return static_cast<UVector::ValueType>(Value); });
}
