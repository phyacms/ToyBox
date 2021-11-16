// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputChord.h"

bool FPulseInput::IsValid() const noexcept
{
	if (!InputCode.IsValid())
	{
		return false;
	}

	if (Event == EPulseInput::RolledDown || Event == EPulseInput::RolledUp)
	{
		return InputCode == FInputCode{ EMouseButton::Middle };
	}

	return true;
}

FInputChord::FInputChord()
	: Trigger{}
	, Modifiers{}
{
}

FInputChord::FInputChord(FInputChord&& Other) noexcept
	: Trigger{ std::move(Other.Trigger) }
	, Modifiers{ std::move(Other.Modifiers) }
{
	Other.Reset();
}

FInputChord& FInputChord::operator=(FInputChord&& Other) & noexcept
{
	if (this != &Other)
	{
		Trigger = std::move(Other.Trigger);
		Modifiers = std::move(Other.Modifiers);
		Other.Reset();
	}
	return *this;
}
