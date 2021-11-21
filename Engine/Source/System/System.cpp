// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "System.h"
#include "Window/SystemWindow.h"
#include "Window/ISystemWindowProcedure.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "Platform/Platform.h"

FSystem::FSystem()
	: Input{}
	, Graphics{}
	, MaximumTickRate{ 100000.0 }
	, MaximumFrameRate{ 300.0 }
{
}

FSystem::~FSystem() noexcept
{
	Graphics.reset();
	Input.reset();
}

void FSystem::RequestAppExit(std::int32_t ExitCode) const noexcept { PlatformFunctions::RequestAppExit(ExitCode); }
void FSystem::PrintDebugOutput(const FString& Message) const noexcept { PlatformFunctions::PrintDebugOutput(Message); }
void FSystem::ShowPopupMessage(const FString& Title, const FString& Content) const noexcept { PlatformFunctions::ShowPopupMessage(Title, Content); }

std::unique_ptr<FSystemWindow> FSystem::CreateWindow(FString Title) const& noexcept
{
	return std::make_unique<FSystemWindow>(
		PlatformFunctions::CreateWindowProcedure(),
		std::move(Title));
}

FInput& FSystem::GetInput() & noexcept
{
	if (Input == nullptr)
	{
		Input = std::make_unique<FInput>(const_cast<FSystem&>(*this));
	}
	return *Input;
}

FGraphics& FSystem::GetGraphics() & noexcept
{
	if (Graphics == nullptr)
	{
		Graphics = std::make_unique<FGraphics>(const_cast<FSystem&>(*this));
	}
	return *Graphics;
}
