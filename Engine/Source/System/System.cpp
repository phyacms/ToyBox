// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "System.h"
#include "Window/SystemWindow.h"
#include "Window/ISystemWindowProcedure.h"
#include "Graphics/Graphics.h"
#include "Platform/Platform.h"

FSystem::FSystem()
	: Graphics{}
{
}

FSystem::~FSystem() noexcept
{
}

void FSystem::RequestAppExit(std::int32_t ExitCode) const noexcept { FPlatform::GetInterface().RequestAppExit(ExitCode); }
void FSystem::PrintDebugOutput(FStringView Message) const noexcept { FPlatform::GetInterface().PrintDebugOutput(Message); }
void FSystem::ShowPopupMessage(FStringView Title, FStringView Content) const noexcept { FPlatform::GetInterface().ShowPopupMessage(Title, Content); }

std::unique_ptr<FSystemWindow> FSystem::CreateWindow(FStringView Title) const noexcept
{
	return std::make_unique<FSystemWindow>(
		FPlatform::GetInterface().CreateWindowProcedure(),
		Title);
}

FGraphics& FSystem::GetGraphics() const noexcept
{
	if (Graphics == nullptr)
	{
		Graphics = std::make_unique<FGraphics>(const_cast<FSystem&>(*this));
	}
	return *Graphics;
}
