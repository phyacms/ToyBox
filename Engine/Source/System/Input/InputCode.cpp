// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputCode.h"

FInputCode::FInputCode(EKeyboardKey Key)
	: Value{ static_cast<std::size_t>(Key) }
{
}

FInputCode::FInputCode(EMouseButton Button)
	: Value{ KeyboardFunctions::KeyCount + static_cast<std::size_t>(Button) }
{
}

FInputCode::operator EKeyboardKey() const noexcept
{
	return IsKeyboardKey()
		? static_cast<EKeyboardKey>(Value)
		: EKeyboardKey::Invalid;
}

FInputCode::operator EMouseButton() const noexcept
{
	return IsMouseButton()
		? static_cast<EMouseButton>(Value - KeyboardFunctions::KeyCount)
		: EMouseButton::Invalid;
}

bool FInputCode::IsValid() const noexcept
{
	std::size_t InputCode{ Value };
	if (InputCode < FInputCode::CodeLimit)
	{
		if (IsKeyboardKey())
		{
			return KeyboardFunctions::IsValidKey(static_cast<EKeyboardKey>(*this));
		}
		else if (IsMouseButton())
		{
			return MouseFunctions::IsValidButton(static_cast<EMouseButton>(InputCode));
		}
	}
	return false;
}

bool FInputCode::IsKeyboardKey() const noexcept
{
	return Value < KeyboardFunctions::KeyCount;
}

bool FInputCode::IsMouseButton() const noexcept
{
	return Value > KeyboardFunctions::KeyCount
		&& (Value - KeyboardFunctions::KeyCount) < MouseFunctions::ButtonCount;
}
