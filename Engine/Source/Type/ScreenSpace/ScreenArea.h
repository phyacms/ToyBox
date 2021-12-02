// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "ScreenLocation.h"
#include "ScreenSize.h"

struct FScreenArea final
{
	FScreenLocation Location{};
	FScreenSize Size{};

	FScreenArea GetInnerAreaOf(float MinimumAspectRatio, float MaximumAspectRatio) const noexcept;
	inline FScreenArea GetInnerAreaOf(float AspectRatio) const noexcept { return GetInnerAreaOf(AspectRatio, AspectRatio); }
};
