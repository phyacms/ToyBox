// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatformEntry.h"

#ifdef PLATFORM_WINDOWS

#include "Application/IApplication.h"

FWindowsPlatformEntry::FWindowsPlatformEntry(HINSTANCE hInstance)
	: hInstance(hInstance)
	, bCoInit(SUCCEEDED(::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED)))
{
#ifndef NDEBUG
	// Enables run-time memory check in debug build.
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

FWindowsPlatformEntry::~FWindowsPlatformEntry() noexcept
{
	if (bCoInit)
	{
		::CoUninitialize();
	}
}

std::int32_t FWindowsPlatformEntry::Launch(IApplication& Application) noexcept
{
	if (!bCoInit || !Application.Initialize())
	{
		return EXIT_FAILURE;
	}

	MSG Msg{};
	while (::GetMessageW(&Msg, nullptr, UINT{}, UINT{}) != FALSE)
	{
		if (Msg.message == WM_QUIT)
		{
			break;
		}
		::TranslateMessage(&Msg);
		::DispatchMessageW(&Msg);
	}

	Application.Terminate();

	return static_cast<std::int32_t>(Msg.wParam);
}

#endif
