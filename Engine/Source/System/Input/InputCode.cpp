// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputCode.h"

FInputCode::FInputCode(EKeyboardKey Key)
	: Index{ static_cast<std::size_t>(Key) }
{
}

FInputCode::FInputCode(EMouseButton Button)
	: Index{ KeyboardFunctions::KeyCount + static_cast<std::size_t>(Button) }
{
}

FInputCode::operator EKeyboardKey() const noexcept
{
	return IsKeyboardKey()
		? static_cast<EKeyboardKey>(Index)
		: EKeyboardKey::Invalid;
}

FInputCode::operator EMouseButton() const noexcept
{
	return IsMouseButton()
		? static_cast<EMouseButton>(Index - KeyboardFunctions::KeyCount)
		: EMouseButton::Invalid;
}

bool FInputCode::IsValid() const noexcept
{
	std::size_t InputCode{ Index };
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
	return Index < KeyboardFunctions::KeyCount;
}

bool FInputCode::IsMouseButton() const noexcept
{
	return Index > KeyboardFunctions::KeyCount
		&& (Index - KeyboardFunctions::KeyCount) < MouseFunctions::ButtonCount;
}
