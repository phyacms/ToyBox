// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "ScreenSpace.h"

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

UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept
{
	UAbsCoord Abs{};
	Abs.Coord = Rel.GetAsVector() * Size.CastAs<float>();
	return Abs;
}

URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept
{
	URelCoord Rel{};
	Rel.Norms.Set<float>(Abs.GetAsVector() / Size.CastAs<float>());
	return Rel;
}

UCoord UCoord::operator-() const noexcept
{
	UAbsCoord Abs{};
	Abs.Coord = -this->Abs.Coord;

	URelCoord Rel{};
	Rel.Norms.Set<float>(-this->Rel.Norms.GetAsVector());

	return UCoord{ std::move(Abs), std::move(Rel) };
}

UCoord& UCoord::operator+=(const UCoord& U) noexcept
{
	Abs.Coord += U.Abs.Coord;
	Rel.Norms.Set<float>(Rel.GetAsVector() + U.Rel.GetAsVector());
	return *this;
}

UCoord& UCoord::operator-=(const UCoord& U) noexcept
{
	Abs.Coord -= U.Abs.Coord;
	Rel.Norms.Set<float>(Rel.GetAsVector() - U.Rel.GetAsVector());
	return *this;
}

UAbsCoord UCoord::GetAsAbsolute(const FScreenSize& Size) const noexcept
{
	UAbsCoord Abs{};
	Abs.Coord = this->Abs.GetAsVector() + ::ToAbsolute(Rel, Size).GetAsVector();
	return Abs;
}

URelCoord UCoord::GetAsRelative(const FScreenSize& Size) const noexcept
{
	URelCoord Rel{};
	Rel.Norms.Set<float>(this->Rel.Norms.GetAsVector() + ::ToRelative(Abs, Size).GetAsVector());
	return Rel.Norms;
}
