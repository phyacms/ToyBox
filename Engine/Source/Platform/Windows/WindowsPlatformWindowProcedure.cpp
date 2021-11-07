// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatformWindowProcedure.h"

#ifdef PLATFORM_WINDOWS

#include "System/Window/SystemWindow.h"

std::size_t WindowsPlatform::FWndProc::RegisterCounter{};

WindowsPlatform::FWndProc::FWndProc()
	: ISystemWindowProcedure()
	, WindowsPlatform::IWindowInterface()
	, bRegistered{ false }
	, hWnd{ nullptr }
{
}

WindowsPlatform::FWndProc::~FWndProc() noexcept
{
	DestroyWindow();
	UnregisterClass();
}

bool WindowsPlatform::FWndProc::IsValidImpl() const noexcept
{
	return bRegistered && hWnd != nullptr;
}

bool WindowsPlatform::FWndProc::InitializeImpl(FSystemWindow& OwnerWindow) noexcept
{
	return RegisterClass() && CreateWindow();
}

void WindowsPlatform::FWndProc::TerminateImpl(FSystemWindow& OwnerWindow) noexcept
{
}

void WindowsPlatform::FWndProc::Present() noexcept
{
	if (IsValid())
	{
		::ShowWindow(hWnd, SW_SHOWDEFAULT);
	}
}

bool WindowsPlatform::FWndProc::RegisterClass() noexcept
{
	bRegistered = true;
	if (RegisterCounter == std::size_t{})
	{
		static const WNDCLASSEX WndClass
		{
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = reinterpret_cast<WNDPROC>(&DefWindowProc),
			.cbClsExtra = int{},
			.cbWndExtra = int{},
			.hInstance = FWindowsPlatform::GetSpecific().GetApplicationHandle(),
			.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION),
			.hCursor = ::LoadCursorW(nullptr, IDC_ARROW),
			.hbrBackground = static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
			.lpszMenuName = nullptr,
			.lpszClassName = ClassName,
			.hIconSm = ::LoadIconW(nullptr, IDI_APPLICATION)
		};
		bRegistered = ::RegisterClassExW(&WndClass) != ATOM{};
	}
	if (bRegistered)
	{
		++RegisterCounter;
	}
	return bRegistered;
}

bool WindowsPlatform::FWndProc::CreateWindow() noexcept
{
	return bRegistered
		? ::CreateWindowExW(
			WS_EX_APPWINDOW,
			ClassName,
			reinterpret_cast<LPCWSTR>(GetWindow().GetTitle().c_str()),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			FWindowsPlatform::GetSpecific().GetApplicationHandle(),
			this) != nullptr
		: false;
}

void WindowsPlatform::FWndProc::UnregisterClass() noexcept
{
	if (bRegistered)
	{
		if (--RegisterCounter == std::size_t{})
		{
			::UnregisterClassW(
				ClassName,
				FWindowsPlatform::GetSpecific().GetApplicationHandle());
		}
		bRegistered = false;
	}
}

void WindowsPlatform::FWndProc::DestroyWindow() noexcept
{
	if (hWnd != nullptr)
	{
		::DestroyWindow(hWnd);
		hWnd = nullptr;
	}
}

LRESULT WindowsPlatform::FWndProc::DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	FWndProc* WndProc{ nullptr };

	if (uMsg == WM_NCCREATE)
	{
		auto CreateStruct{ reinterpret_cast<CREATESTRUCTW*>(lParam) };
		WndProc = reinterpret_cast<FWndProc*>(CreateStruct->lpCreateParams);
		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WndProc));

		WndProc->hWnd = hWnd;
	}
	else
	{
		WndProc = reinterpret_cast<FWndProc*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (WndProc != nullptr)
	{
		if (uMsg == WM_DESTROY)
		{
			::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
		}

		if (WndProc->IsValid())
		{
			return WndProc->ProcMessage(uMsg, wParam, lParam);
		}
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT WindowsPlatform::FWndProc::ProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	using namespace SystemWindowEvents;

	FSystemWindow& Window{ GetWindow() };

	switch (uMsg)
	{
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				FOnResized OnResized{
					.Width = static_cast<std::uint32_t>(LOWORD(lParam)),
					.Height = static_cast<std::uint32_t>(HIWORD(lParam)) };
				if (OnResized.Width != 0 && OnResized.Height != 0)
				{
					Window.Events.Enqueue(std::move(OnResized));
				}
			}
		}
		return 0;

		case WM_CLOSE:
		{
			Window.Events.Enqueue(FOnClosed{});
		}
		return 0;
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

#endif
