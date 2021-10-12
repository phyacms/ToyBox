// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatform.h"

#ifdef PLATFORM_WINDOWS

IPlatform& FWindowsPlatform::GetInstance() noexcept
{
	static FWindowsPlatform Instance{};
	return Instance;
}

#endif
