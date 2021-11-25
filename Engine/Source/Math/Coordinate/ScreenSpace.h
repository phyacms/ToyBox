// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Math/Vector.h"
#include "Math/Point.h"

using FScreenLocation = TPoint<std::int32_t, 2>;
using FScreenSize = TVector<std::uint32_t, 2>;
struct FScreenArea final
{
	FScreenLocation Location{};
	FScreenSize Size{};
};
