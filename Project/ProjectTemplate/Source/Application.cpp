// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>
#include <System/Window/SystemWindow.h>

FApplication::FApplication(FSystem& System)
	: IApplication(System)
	, Window{}
{
}

FApplication::~FApplication() noexcept
{
}

bool FApplication::Initialize(const FCommandLineArgs& CmdLine) noexcept
{
	Window = GetSystem().CreateWindow(USTR(PROJECT_NAME));
	if (Window == nullptr || !Window->IsValid())
	{
		return false;
	}

	WindowEvents += Window->Events.OnClosed.AddDynamic(
		[this](const SystemWindowEvents::FOnClosed&)->bool { GetSystem().RequestAppExit(EXIT_SUCCESS); return true; });

	Window->Present();

	return true;
}

void FApplication::Terminate() noexcept
{
	WindowEvents.Clear();
	Window.reset();
}

void FApplication::Tick(FTimeDuration DeltaTime)
{
	Window->Events.Process();
}

void FApplication::Render(FTimeDuration DeltaTime) const
{
}
