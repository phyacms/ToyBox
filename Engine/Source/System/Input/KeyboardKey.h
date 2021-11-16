// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SwitchState.h"

enum class EKeyboardKey : std::size_t
{
	Invalid = std::size_t{},

	Ctrl, Alt, Shift,
	Up, Down, Left, Right,
	Escape, Enter, Space, Tab, Backspace,
	Insert, Delete, Home, End, PageUp, PageDown,
	PrintScreen, ScrollLock, Pause,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	OpenBracket, CloseBracket, Semicolon, Quotes, Comma, Period, Question, Backslash,
	CapsLock, NumLock,
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Minus, Plus, Tilde,
	NumPad0, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
	NumPadAdd, NumPadSubtract, NumPadMultiply, NumPadDivide, NumPadDecimal,
	SystemCommand,

	ENUM_END
};

namespace KeyboardFunctions
{
	constexpr std::size_t KeyCount{ static_cast<std::size_t>(EKeyboardKey::ENUM_END) };

	inline bool IsValidKey(EKeyboardKey KeyCode) noexcept { return KeyCode > EKeyboardKey::Invalid && KeyCode < EKeyboardKey::ENUM_END; }
	inline std::size_t KeyCodeToIndex(EKeyboardKey KeyCode) noexcept { return IsValidKey(KeyCode) ? static_cast<std::size_t>(KeyCode) : KeyCount; }

	inline bool IsModifierKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::Ctrl && KeyCode <= EKeyboardKey::Shift; }
	inline bool IsArrowKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::Up && KeyCode <= EKeyboardKey::Right; }
	inline bool IsNavigationKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::Insert && KeyCode <= EKeyboardKey::PageDown; }
	inline bool IsFunctionKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::F1 && KeyCode <= EKeyboardKey::F12; }
	inline bool IsToggleKey(EKeyboardKey KeyCode) noexcept { return (KeyCode >= EKeyboardKey::CapsLock && KeyCode <= EKeyboardKey::NumLock) || KeyCode == EKeyboardKey::ScrollLock; }
	inline bool IsSpecialPurposeKey(EKeyboardKey KeyCode) noexcept { return KeyCode == EKeyboardKey::SystemCommand || KeyCode == EKeyboardKey::PrintScreen; }

	inline bool IsNumPadNumberKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::NumPad0 && KeyCode <= EKeyboardKey::NumPad9; }
	inline bool IsNumPadSymbolKey(EKeyboardKey KeyCode) noexcept { return KeyCode >= EKeyboardKey::NumPadAdd && KeyCode <= EKeyboardKey::NumPadDecimal; }
	inline bool IsNumPadKey(EKeyboardKey KeyCode) noexcept { return IsNumPadNumberKey(KeyCode) || IsNumPadSymbolKey(KeyCode) || KeyCode == EKeyboardKey::NumLock; }

	inline bool IsNumberKey(EKeyboardKey KeyCode, bool bIncludeNumPad = false) noexcept
	{
		bool bNumberKey = KeyCode >= EKeyboardKey::Num0 && KeyCode <= EKeyboardKey::Num9;
		if (bIncludeNumPad)
		{
			bNumberKey |= IsNumPadNumberKey(KeyCode);
		}
		return bNumberKey;
	}
	inline bool IsSymbolKey(EKeyboardKey KeyCode, bool bIncludeNumPad = false) noexcept
	{
		bool bSymbolKey
			= (KeyCode >= EKeyboardKey::OpenBracket && KeyCode <= EKeyboardKey::Backslash)
			|| (KeyCode >= EKeyboardKey::Minus && KeyCode <= EKeyboardKey::Tilde);
		if (bIncludeNumPad)
		{
			bSymbolKey |= IsNumPadSymbolKey(KeyCode);
		}
		return bSymbolKey;
	}

	inline bool IsTypingKey(EKeyboardKey KeyCode, bool bIncludeNumPad = false) noexcept { return IsNumberKey(KeyCode, bIncludeNumPad) || IsSymbolKey(KeyCode, bIncludeNumPad); }
}

using FKeyboardKeyStates = std::array<ESwitchState, KeyboardFunctions::KeyCount>;
