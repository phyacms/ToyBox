// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

enum class EMouseWheel : std::size_t
{
	RollUp,
	RollDown,
};

using FMouseWheelDelta = std::vector<EMouseWheel>;
