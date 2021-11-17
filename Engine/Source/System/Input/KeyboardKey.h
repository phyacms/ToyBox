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

namespace InputFunctions
{
	constexpr std::size_t KeyboardKeyCount{ static_cast<std::size_t>(EKeyboardKey::ENUM_END) };

	inline bool IsValidKey(EKeyboardKey Key) noexcept { return Key > EKeyboardKey::Invalid && Key < EKeyboardKey::ENUM_END; }
	inline std::size_t ToIndex(EKeyboardKey Key) noexcept { return static_cast<std::size_t>(Key); }

	inline bool IsModifierKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::Ctrl && Key <= EKeyboardKey::Shift; }
	inline bool IsArrowKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::Up && Key <= EKeyboardKey::Right; }
	inline bool IsNavigationKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::Insert && Key <= EKeyboardKey::PageDown; }
	inline bool IsFunctionKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::F1 && Key <= EKeyboardKey::F12; }
	inline bool IsToggleKey(EKeyboardKey Key) noexcept { return (Key >= EKeyboardKey::CapsLock && Key <= EKeyboardKey::NumLock) || Key == EKeyboardKey::ScrollLock; }
	inline bool IsSpecialPurposeKey(EKeyboardKey Key) noexcept { return Key == EKeyboardKey::SystemCommand || Key == EKeyboardKey::PrintScreen; }

	inline bool IsNumPadNumberKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::NumPad0 && Key <= EKeyboardKey::NumPad9; }
	inline bool IsNumPadSymbolKey(EKeyboardKey Key) noexcept { return Key >= EKeyboardKey::NumPadAdd && Key <= EKeyboardKey::NumPadDecimal; }
	inline bool IsNumPadKey(EKeyboardKey Key) noexcept { return IsNumPadNumberKey(Key) || IsNumPadSymbolKey(Key) || Key == EKeyboardKey::NumLock; }

	inline bool IsNumberKey(EKeyboardKey Key, bool bIncludeNumPad = false) noexcept
	{
		bool bNumberKey = Key >= EKeyboardKey::Num0 && Key <= EKeyboardKey::Num9;
		if (bIncludeNumPad)
		{
			bNumberKey |= IsNumPadNumberKey(Key);
		}
		return bNumberKey;
	}
	inline bool IsSymbolKey(EKeyboardKey Key, bool bIncludeNumPad = false) noexcept
	{
		bool bSymbolKey
			= (Key >= EKeyboardKey::OpenBracket && Key <= EKeyboardKey::Backslash)
			|| (Key >= EKeyboardKey::Minus && Key <= EKeyboardKey::Tilde);
		if (bIncludeNumPad)
		{
			bSymbolKey |= IsNumPadSymbolKey(Key);
		}
		return bSymbolKey;
	}

	inline bool IsTypingKey(EKeyboardKey Key, bool bIncludeNumPad = false) noexcept { return IsNumberKey(Key, bIncludeNumPad) || IsSymbolKey(Key, bIncludeNumPad); }
}

using FKeyboardKeyStates = std::array<ESwitchState, InputFunctions::KeyboardKeyCount>;
