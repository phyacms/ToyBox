// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "RelCoord.h"
#include "AbsCoord.h"

URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept
{
	URelCoord Rel{};
	Rel.Norms.Set<float>(Abs.GetAsVector() / Size.CastAs<float>());
	return Rel;
}
