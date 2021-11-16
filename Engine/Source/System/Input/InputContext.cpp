// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputContext.h"

FInputContext::FInputContext(
	FInput& Input,
	AObject<FSystemWindow>&& InputWindow)
	: Input{ &Input }
	, InputWindow{ std::move(InputWindow) }
{
}

FInputContext::~FInputContext() noexcept
{
	InputWindow.Release();
}

bool FInputContext::IsValid() const noexcept
{
	return InputWindow.IsValid();
}
