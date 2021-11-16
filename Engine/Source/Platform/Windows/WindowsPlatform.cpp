// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatform.h"

#ifdef PLATFORM_WINDOWS

#include "Platform/Platform.h"
#include "WindowsPlatformWindowProcedure.h"
#include "System/Input/InputCode.h"

void PlatformFunctions::RequestAppExit(std::int32_t ExitCode) noexcept
{
	::PostThreadMessageW(
		WindowsPlatform::GetMessageThreadId(),
		WM_QUIT,
		static_cast<WPARAM>(ExitCode),
		LPARAM{});
}

void PlatformFunctions::PrintDebugOutput(FStringView Message) noexcept
{
	static std::mutex Mutex{};
	std::unique_lock<std::mutex> Lock{ Mutex };
	::OutputDebugStringW(reinterpret_cast<LPCWSTR>(Message.GetPtr()));
	::OutputDebugStringW(TEXT("\n"));
}

void PlatformFunctions::ShowPopupMessage(FStringView Title, FStringView Content) noexcept
{
	::MessageBoxW(
		nullptr,
		reinterpret_cast<LPCWSTR>(Content.GetPtr()),
		reinterpret_cast<LPCWSTR>(Title.GetPtr()),
		MB_OK);
}

std::unique_ptr<ISystemWindowProcedure> PlatformFunctions::CreateWindowProcedure()
{
	return std::make_unique<WindowsPlatform::FWndProc>();
}

std::optional<std::vector<char8_t>> PlatformFunctions::StringToUTF8(FStringView Str)
{
	auto RequiredBufferSize{ ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<LPCWSTR>(Str.GetPtr()),
		-1,
		nullptr,
		0,
		nullptr,
		nullptr) - 1 };

	std::vector<char8_t> Multibyte(RequiredBufferSize, u8'\0');
	const auto BytesWritten{ ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<LPCWSTR>(Str.GetPtr()),
		static_cast<int>(Str.Length<EStringLength::CodePoint>()),
		reinterpret_cast<LPCH>(Multibyte.data()),
		static_cast<int>(Multibyte.size()),
		nullptr,
		nullptr) };
	if (BytesWritten == 0)
	{
		return std::nullopt;
	}

	return std::vector<char8_t>{
		std::cbegin(Multibyte),
		std::find(
			std::cbegin(Multibyte),
			std::cend(Multibyte),
			u8'\0') };
}

std::optional<FString> PlatformFunctions::UTF8ToString(const std::vector<char8_t>& u8Chars)
{
	auto RequiredCharacterSize{ ::MultiByteToWideChar(
		CP_UTF8,
		DWORD{},
		reinterpret_cast<LPCCH>(u8Chars.data()),
		-1,
		nullptr,
		0) - 1 };

	std::u16string u16Chars(RequiredCharacterSize + std::size_t{ 1 }, u'\0');
	const auto CharactersWritten{ ::MultiByteToWideChar(
		CP_UTF8,
		DWORD{},
		reinterpret_cast<LPCCH>(u8Chars.data()),
		static_cast<int>(u8Chars.size()),
		reinterpret_cast<LPWSTR>(u16Chars.data()),
		static_cast<int>(u16Chars.size())) };
	if (CharactersWritten == 0)
	{
		return std::nullopt;
	}

	return FString(u16Chars.data(), u16Chars.find_first_of(u'\0'));
}

EKeyboardKey WindowsPlatform::TranslateKeyboardKey(int VirtualKey) noexcept
{
	switch (VirtualKey)
	{
		default: return EKeyboardKey::Invalid;
		case VK_CONTROL: return EKeyboardKey::Ctrl;
		case VK_MENU: return EKeyboardKey::Alt;
		case VK_SHIFT: return EKeyboardKey::Shift;
		case VK_UP: return EKeyboardKey::Up;
		case VK_DOWN: return EKeyboardKey::Down;
		case VK_LEFT: return EKeyboardKey::Left;
		case VK_RIGHT: return EKeyboardKey::Right;
		case VK_ESCAPE: return EKeyboardKey::Escape;
		case VK_RETURN: return EKeyboardKey::Enter;
		case VK_SPACE: return EKeyboardKey::Space;
		case VK_TAB: return EKeyboardKey::Tab;
		case VK_BACK: return EKeyboardKey::Backspace;
		case VK_INSERT: return EKeyboardKey::Insert;
		case VK_DELETE: return EKeyboardKey::Delete;
		case VK_HOME: return EKeyboardKey::Home;
		case VK_END: return EKeyboardKey::End;
		case VK_PRIOR: return EKeyboardKey::PageUp;
		case VK_NEXT: return EKeyboardKey::PageDown;
		case VK_SNAPSHOT: return EKeyboardKey::PrintScreen;
		case VK_SCROLL: return EKeyboardKey::ScrollLock;
		case VK_PAUSE: return EKeyboardKey::Pause;
		case VK_LWIN: return EKeyboardKey::SystemCommand;
		case VK_NUMLOCK: return EKeyboardKey::NumLock;
		case VK_CAPITAL: return EKeyboardKey::CapsLock;
		case VK_F1: return EKeyboardKey::F1;
		case VK_F2: return EKeyboardKey::F2;
		case VK_F3: return EKeyboardKey::F3;
		case VK_F4: return EKeyboardKey::F4;
		case VK_F5: return EKeyboardKey::F5;
		case VK_F6: return EKeyboardKey::F6;
		case VK_F7: return EKeyboardKey::F7;
		case VK_F8: return EKeyboardKey::F8;
		case VK_F9: return EKeyboardKey::F9;
		case VK_F10: return EKeyboardKey::F10;
		case VK_F11: return EKeyboardKey::F11;
		case VK_F12: return EKeyboardKey::F12;
		case 0x41: return EKeyboardKey::A;
		case 0x42: return EKeyboardKey::B;
		case 0x43: return EKeyboardKey::C;
		case 0x44: return EKeyboardKey::D;
		case 0x45: return EKeyboardKey::E;
		case 0x46: return EKeyboardKey::F;
		case 0x47: return EKeyboardKey::G;
		case 0x48: return EKeyboardKey::H;
		case 0x49: return EKeyboardKey::I;
		case 0x4A: return EKeyboardKey::J;
		case 0x4B: return EKeyboardKey::K;
		case 0x4C: return EKeyboardKey::L;
		case 0x4D: return EKeyboardKey::M;
		case 0x4E: return EKeyboardKey::N;
		case 0x4F: return EKeyboardKey::O;
		case 0x50: return EKeyboardKey::P;
		case 0x51: return EKeyboardKey::Q;
		case 0x52: return EKeyboardKey::R;
		case 0x53: return EKeyboardKey::S;
		case 0x54: return EKeyboardKey::T;
		case 0x55: return EKeyboardKey::U;
		case 0x56: return EKeyboardKey::V;
		case 0x57: return EKeyboardKey::W;
		case 0x58: return EKeyboardKey::X;
		case 0x59: return EKeyboardKey::Y;
		case 0x5A: return EKeyboardKey::Z;
		case VK_OEM_4: return EKeyboardKey::OpenBracket;
		case VK_OEM_6: return EKeyboardKey::CloseBracket;
		case VK_OEM_1: return EKeyboardKey::Semicolon;
		case VK_OEM_7: return EKeyboardKey::Quotes;
		case VK_OEM_COMMA: return EKeyboardKey::Comma;
		case VK_OEM_PERIOD: return EKeyboardKey::Period;
		case VK_OEM_2: return EKeyboardKey::Question;
		case VK_OEM_5: return EKeyboardKey::Backslash;
		case 0x30: return EKeyboardKey::Num0;
		case 0x31: return EKeyboardKey::Num1;
		case 0x32: return EKeyboardKey::Num2;
		case 0x33: return EKeyboardKey::Num3;
		case 0x34: return EKeyboardKey::Num4;
		case 0x35: return EKeyboardKey::Num5;
		case 0x36: return EKeyboardKey::Num6;
		case 0x37: return EKeyboardKey::Num7;
		case 0x38: return EKeyboardKey::Num8;
		case 0x39: return EKeyboardKey::Num9;
		case VK_OEM_PLUS: return EKeyboardKey::Plus;
		case VK_OEM_MINUS: return EKeyboardKey::Minus;
		case VK_OEM_3: return EKeyboardKey::Tilde;
		case VK_NUMPAD0: return EKeyboardKey::NumPad0;
		case VK_NUMPAD1: return EKeyboardKey::NumPad1;
		case VK_NUMPAD2: return EKeyboardKey::NumPad2;
		case VK_NUMPAD3: return EKeyboardKey::NumPad3;
		case VK_NUMPAD4: return EKeyboardKey::NumPad4;
		case VK_NUMPAD5: return EKeyboardKey::NumPad5;
		case VK_NUMPAD6: return EKeyboardKey::NumPad6;
		case VK_NUMPAD7: return EKeyboardKey::NumPad7;
		case VK_NUMPAD8: return EKeyboardKey::NumPad8;
		case VK_NUMPAD9: return EKeyboardKey::NumPad9;
		case VK_ADD: return EKeyboardKey::NumPadAdd;
		case VK_SUBTRACT: return EKeyboardKey::NumPadSubtract;
		case VK_MULTIPLY: return EKeyboardKey::NumPadMultiply;
		case VK_DIVIDE: return EKeyboardKey::NumPadDivide;
		case VK_DECIMAL: return EKeyboardKey::NumPadDecimal;
	}
}

EMouseButton WindowsPlatform::TranslateMouseButton(int VirtualKey) noexcept
{
	switch (VirtualKey)
	{
		default: return EMouseButton::Invalid;
		case VK_LBUTTON: return EMouseButton::Left;
		case VK_RBUTTON: return EMouseButton::Right;
		case VK_MBUTTON: return EMouseButton::Middle;
		case VK_XBUTTON1: return EMouseButton::Back;
		case VK_XBUTTON2: return EMouseButton::Front;
	}
}

#endif
