// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputTrigger.h"

bool InputFunctions::IsValidTrigger(const FInputTrigger& Trigger) noexcept
{
	if (auto Ptr{ std::get_if<FInputCodeTrigger>(&Trigger) };
		Ptr != nullptr)
	{
		return Ptr->IsValid();
	}
	else
	{
		return true;
	}
}
