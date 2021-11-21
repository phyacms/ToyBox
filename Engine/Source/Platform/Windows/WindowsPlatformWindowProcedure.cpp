// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatformWindowProcedure.h"

#ifdef PLATFORM_WINDOWS

#include "System/Window/SystemWindow.h"
#include "System/Input/SwitchState.h"
#include "System/Input/InputTrigger.h"

std::size_t WindowsPlatform::FWndProc::RegisterCount{};

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

void WindowsPlatform::FWndProc::Close() noexcept
{
	::SendMessageW(hWnd, WM_CLOSE, 0, 0);
}

bool WindowsPlatform::FWndProc::RegisterClass() noexcept
{
	bRegistered = true;
	if (RegisterCount == 0)
	{
		static const WNDCLASSEX WndClass
		{
			.cbSize{ sizeof(WNDCLASSEX) },
			.style{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC },
			.lpfnWndProc{ reinterpret_cast<WNDPROC>(&DefWindowProc) },
			.cbClsExtra{},
			.cbWndExtra{},
			.hInstance{ WindowsPlatform::GetApplicationHandle() },
			.hIcon{ ::LoadIconW(nullptr, IDI_APPLICATION) },
			.hCursor{ ::LoadCursorW(nullptr, IDC_ARROW) },
			.hbrBackground{ static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)) },
			.lpszMenuName{ nullptr },
			.lpszClassName{ ClassName },
			.hIconSm{ ::LoadIconW(nullptr, IDI_APPLICATION) }
		};
		bRegistered = ::RegisterClassExW(&WndClass) != ATOM{};
	}
	if (bRegistered)
	{
		++RegisterCount;
	}
	return bRegistered;
}

bool WindowsPlatform::FWndProc::CreateWindow() noexcept
{
	return bRegistered
		? ::CreateWindowExW(
			WS_EX_APPWINDOW,
			ClassName,
			reinterpret_cast<LPCWSTR>(GetWindow().GetTitle().GetPtr()),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			WindowsPlatform::GetApplicationHandle(),
			this) != nullptr
		: false;
}

void WindowsPlatform::FWndProc::UnregisterClass() noexcept
{
	if (bRegistered)
	{
		if (--RegisterCount == 0)
		{
			::UnregisterClassW(
				ClassName,
				WindowsPlatform::GetApplicationHandle());
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
	using namespace SystemWindowEventTypes;

	static constexpr const FMessageHook DefaultHooks[]
	{
		&FWndProc::ProcKeyboardMessage,
		&FWndProc::ProcMouseMessage
	};

	for (const auto& Hook : DefaultHooks)
	{
		if (auto [bHandled, Result] = (this->*Hook)(uMsg, wParam, lParam);
			bHandled)
		{
			return Result;
		}
	}

	auto& Window{ GetWindow() };
	switch (uMsg)
	{
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				FOnResized OnResized{
					.ClientAreaSize{ FScreenSize{
						static_cast<std::uint32_t>(LOWORD(lParam)),
						static_cast<std::uint32_t>(HIWORD(lParam)) } } };
				if (OnResized.ClientAreaSize.Multiplication() != 0)
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

WindowsPlatform::FWndProc::FResult WindowsPlatform::FWndProc::ProcKeyboardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	using namespace SystemWindowEventTypes;

	auto& Window{ GetWindow() };

	bool bPulse{ true };
	auto KeyState{ ESwitchState::Up };
	switch (uMsg)
	{
		default:
			return {};

		// Blocks Alt+Enter beeping.
		case WM_MENUCHAR:
			return { true, MAKELRESULT(0, MNC_CLOSE) };

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			KeyState = ESwitchState::Down;
			bPulse = !(lParam & (1 << 30));
			[[fallthrough]];
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (bPulse)
			{
				if (auto Key{ WindowsPlatform::TranslateKeyboardKey(LOWORD(wParam)) };
					InputFunctions::IsValidKey(Key))
				{
					Window.Events.Enqueue(FOnKeyboardKey{ .Key{ Key }, .State{ KeyState } });
				}
			}
		}
		return { true, 0 };
	}
}

WindowsPlatform::FWndProc::FResult WindowsPlatform::FWndProc::ProcMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	using namespace SystemWindowEventTypes;

	auto& Window{ GetWindow() };
	auto ButtonState{ ESwitchState::Up };
	std::stack<FEvent> Stack{};
	switch (uMsg)
	{
		default:
			return {};

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
			ButtonState = ESwitchState::Down;
			[[fallthrough]];
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			static constexpr auto GetVirtualKey{
				[](UINT uMsg, WPARAM wParam)->int
				{
					switch (uMsg)
					{
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
							return VK_LBUTTON;

						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
							return VK_RBUTTON;

						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
							return VK_MBUTTON;

						case WM_XBUTTONDOWN:
						case WM_XBUTTONUP:
							switch (GET_XBUTTON_WPARAM(wParam))
							{
								case XBUTTON1: return VK_XBUTTON1;
								case XBUTTON2: return VK_XBUTTON2;
								default:
									break;
							}
							break;

						default:
							break;
					}
					return {};
				} };
			if (auto Button{ WindowsPlatform::TranslateMouseButton(GetVirtualKey(uMsg, wParam)) };
				InputFunctions::IsValidButton(Button))
			{
				Stack.push(FOnMouseButton{ .Button{ Button }, .State{ ButtonState } });
			}
		}
		break;

		case WM_MOUSEWHEEL:
		{
			FOnMouseWheel WheelMove{};

			auto WheelDelta{ GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA };
			if (WheelDelta < 0)
			{
				while (WheelDelta++ != 0)
				{
					WheelMove.WheelDelta.emplace_back(EMouseWheelTrigger::RollDown);
				}
			}
			else if (WheelDelta > 0)
			{
				while (WheelDelta-- != 0)
				{
					WheelMove.WheelDelta.emplace_back(EMouseWheelTrigger::RollUp);
				}
			}

			if (!WheelMove.WheelDelta.empty())
			{
				Stack.push(std::move(WheelMove));
			}
		}
		break;

		case WM_MOUSEMOVE:
		case WM_MOUSEHOVER:
		case WM_MOUSEHWHEEL:
			break;
	}

	Window.Events.Enqueue(
		FOnMouseMove{
			.CursorLocation{ FScreenLocation{
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam) } } });
	while (!Stack.empty())
	{
		Window.Events.Enqueue(std::move(Stack.top()));
		Stack.pop();
	}

	return { true, 0 };
}

#endif
