// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"

#ifdef PLATFORM_WINDOWS

#include <Platform/Windows/WindowsPlatformEntry.h>
#include "Application.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPWSTR lpCmdLine,
	[[maybe_unused]] _In_ int nCmdShow)
{
	FWindowsPlatformEntry Entry{ hInstance };
	return static_cast<int>(Entry.Launch<FApplication>());
}

#endif
