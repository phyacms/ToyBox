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

void FWindowsPlatform::PrintDebugOutput(FStringView Message) const noexcept
{
	static std::mutex Mutex{};
	std::unique_lock<std::mutex> Lock{ Mutex };
	::OutputDebugStringW(reinterpret_cast<LPCWSTR>(Message.GetStr()));
	::OutputDebugStringW(TEXT("\n"));
}

std::unique_ptr<ISystemWindowProcedure> FWindowsPlatform::CreateWindowProcedure() const noexcept
{
	return std::make_unique<WindowsPlatform::FWndProc>();
}

#endif
