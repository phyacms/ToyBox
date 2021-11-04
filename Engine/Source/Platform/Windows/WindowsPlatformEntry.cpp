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

HINSTANCE FWindowsPlatform::GetApplicationHandle() const noexcept { return ::g_hInstance; }
DWORD FWindowsPlatform::GetMessageThreadId() const noexcept { return ::g_MessageThreadId; }

FWindowsPlatformEntry::FWindowsPlatformEntry(HINSTANCE hInstance)
	: bCoInit{}
{
	if (::g_hInstance == nullptr)
	{
		::g_hInstance = hInstance;

		bCoInit = SUCCEEDED(::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED));

#ifndef NDEBUG
		// Enables run-time memory check in debug build.
		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		::_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

		::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}
}

FWindowsPlatformEntry::~FWindowsPlatformEntry() noexcept
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
					// In Windows platform, LPWSTR is the pointer of WCHAR and WCHAR is the alias of wchar_t
					// and the size of wchar_t is guaranteed to be 16-bit wide.
					// As the parsed LPWSTR command-line arguments are represented in UCS-2, i.e. UTF-16LE,
					// it seems safe enough to cast the type of a parsed argument from LPWSTR to const char16_t*
					// and store it in FString class instance without any further conversion.
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
	if (::g_MessageThreadId == DWORD{} && bCoInit)
	{
		::g_MessageThreadId = ::GetCurrentThreadId();
		struct FMessageThreadIdGuard final
		{
			~FMessageThreadIdGuard() noexcept { ::g_MessageThreadId = DWORD{}; }
		}
		MessageThreadIdGuard{};

		auto EntryGuard{ CreateEntryGuard(Application) };
		if (EntryGuard.IsInitialized())
		{
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

			return static_cast<std::int32_t>(Msg.wParam);
		}
	}

	return EXIT_FAILURE;
}

#endif
