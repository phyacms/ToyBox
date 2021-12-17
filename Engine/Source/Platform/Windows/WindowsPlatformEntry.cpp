// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatformEntry.h"

#ifdef PLATFORM_WINDOWS

#include "Application/IApplication.h"
#include "System/System.h"

namespace
{
	HINSTANCE g_hInstance{};
	DWORD g_MessageThreadId{};
}

HINSTANCE WindowsPlatform::GetApplicationHandle() noexcept { return ::g_hInstance; }
DWORD WindowsPlatform::GetMessageThreadId() noexcept { return ::g_MessageThreadId; }

FWindowsPlatformEntry::FWindowsPlatformEntry(HINSTANCE hInstance)
	: bCoInit{}
{
	::g_hInstance = hInstance;
	::g_MessageThreadId = ::GetCurrentThreadId();
}

FWindowsPlatformEntry::~FWindowsPlatformEntry() noexcept
{
}

bool FWindowsPlatformEntry::Initialize() noexcept
{
	if (FAILED(::CoInitializeEx(
		nullptr,
		COINIT::COINIT_APARTMENTTHREADED | COINIT::COINIT_DISABLE_OLE1DDE)))
	{
		return false;
	}
	bCoInit = true;

#ifndef NDEBUG
	// Enables run-time memory check in debug build.
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	return true;
}

void FWindowsPlatformEntry::Terminate() noexcept
{
	if (bCoInit)
	{
		::CoUninitialize();
	}
}

FCommandLineArgs FWindowsPlatformEntry::ParseCommandLine() const noexcept
{
	FCommandLineArgs CmdLine{};
	struct FCommandLineParser final
	{
		int Argc{};
		LPWSTR* Argv{ nullptr };
		FCommandLineParser(FCommandLineArgs& Parsed)
		{
			Parsed.clear();
			Argv = ::CommandLineToArgvW(::GetCommandLineW(), &Argc);
			if (Argv != nullptr)
			{
				const auto Count{ static_cast<std::size_t>(Argc) };
				Parsed.reserve(Count);
				for (std::size_t Index{}; Index != Count; ++Index)
				{
					// In Windows platform, it is guaranteed that sizeof(wchar_t) == 16, and therefore
					// it seems to be safe enough to cast parsed argument type to const char16_t.
					// As the parsed command-line arguments are encoded in UTF-16LE,
					// FString can store them w/o any further type conversion or code page conversion.
					Parsed.emplace_back(reinterpret_cast<const char16_t*>(Argv[Index]));
				}
				Parsed.shrink_to_fit();
			}
		}
		~FCommandLineParser() noexcept
		{
			if (Argv != nullptr)
			{
				::LocalFree(Argv);
			}
		}
	}
	Parser{ CmdLine };
	return CmdLine;
}

std::int32_t FWindowsPlatformEntry::AppMain(IApplication& Application)
{
	const auto& EntryGuard{ CreateEntryGuard(Application) };
	if (EntryGuard.IsInitialized())
	{
		MSG Msg{};
		while (::GetMessageW(&Msg, nullptr, 0, 0) != FALSE)
		{
			if (Msg.message == WM_QUIT)
			{
				break;
			}
			::TranslateMessage(&Msg);
			::DispatchMessageW(&Msg);
		}
		return static_cast<std::int32_t>(Msg.wParam);
	}
	return EXIT_FAILURE;
}

#endif
