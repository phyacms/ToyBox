// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Coord.h"
#include "ScreenArea.h"

struct URect final
{
	static const URect Default;

	UCoord Location{};
	UCoord Size{};
};

inline FScreenArea ToScreenSpace(const URect& Rect, const FScreenArea& Area)
{
	const auto& Offset{ Rect.Location.GetAsAbsolute(Area.Size).GetAsVector() };
	const auto& Diagonal{ Rect.Size.GetAsAbsolute(Area.Size).GetAsVector() };
	return {
		.Location{ FScreenLocation{
			Area.Location.X() + Offset.X(),
			Area.Location.Y() + Offset.Y() } },
		.Size{ FScreenSize{
			Diagonal.X(),
			Diagonal.Y() } } };
}
