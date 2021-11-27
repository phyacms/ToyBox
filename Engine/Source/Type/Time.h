// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

using FTimeClock = std::chrono::steady_clock;
static_assert(FTimeClock::is_steady);
using FTimePoint = FTimeClock::time_point;

using FTimeRep = double;
using FTimePeriod = std::milli;
using FTimeDuration = std::chrono::duration<FTimeRep, FTimePeriod>;

struct FTimeStamp
{
	FTimePoint Time;
	FTimeStamp() : Time{ FTimeClock::now() } {}
};
