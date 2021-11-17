// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputChord.h"

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

bool FInputChord::IsValid() const noexcept
{
	if (!InputFunctions::IsValidTrigger(Trigger))
	{
		return false;
	}

	return std::visit(stdhelp::overloaded{
		[](const auto&)->bool { return true; },
		[this](const FInputCodeTrigger& Trigger)->bool {
			return !Modifiers.InputCodes.contains(Trigger.InputCode); }, },
		Trigger);

}

FInputChord& FInputChord::SetTrigger(FInputTrigger Trigger) noexcept
{
	if (InputFunctions::IsValidTrigger(Trigger))
	{
		this->Trigger = std::move(Trigger);
	}
	return *this;
}

FInputChord& FInputChord::AddModifier(FInputCode InputCode) noexcept
{
	if (InputFunctions::IsValidInputCode(InputCode))
	{
		Modifiers += InputCode;
	}
	return *this;
}

FInputChord& FInputChord::RemoveModifier(FInputCode InputCode) noexcept
{
	if (InputFunctions::IsValidInputCode(InputCode))
	{
		Modifiers -= InputCode;
	}
	return *this;
}
