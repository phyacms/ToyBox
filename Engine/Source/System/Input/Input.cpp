// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Input.h"
#include "InputContext.h"

FInput::FInput(FSystem& System)
	: System{ &System }
{
}

FInput::~FInput() noexcept
{
}

std::unique_ptr<FInputContext> FInput::CreateContext(FSystemWindow& InputWindow) &
{
	auto Context{ std::make_unique<FInputContext>(*this, InputWindow) };
	if (Context != nullptr && Context->IsValid())
	{
		return Context;
	}

	return nullptr;
}
