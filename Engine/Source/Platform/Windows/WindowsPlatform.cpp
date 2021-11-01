// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatform.h"

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatformWindowProcedure.h"

IPlatform& FWindowsPlatform::GetInterface() noexcept
{
	return GetSpecific();
}

FWindowsPlatform& FWindowsPlatform::GetSpecific() noexcept
{
	static FWindowsPlatform Instance{};
	return Instance;
}

std::unique_ptr<ISystemWindowProcedure> FWindowsPlatform::CreateWindowProcedure() const noexcept
{
	return std::make_unique<WindowsPlatform::Detail::FWndProc>();
}

#endif
