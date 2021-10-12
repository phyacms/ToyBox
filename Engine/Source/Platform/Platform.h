// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsPlatform.h"
using FPlatform = FWindowsPlatform;
#else
static_assert(false);
#endif
