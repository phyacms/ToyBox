// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>
#include <System/Window/SystemWindow.h>
#include <System/Input/Input.h>
#include <System/Input/InputContext.h>
#include <System/Graphics/Graphics.h>
#include <System/Graphics/IGraphicsContext.h>

FApplication::FApplication(FSystem& System)
	: IApplication(System)
	, Window{}
	, DH_OnClosed{}
	, Input{}
	, Graphics{}
{
}

FApplication::~FApplication() noexcept
{
}

bool FApplication::Initialize(const FCommandLineArgs& CmdLine) noexcept
{
	FSystem& System{ GetSystem() };

	Window = System.CreateWindow(USTR(PROJECT_NAME));
	if (Window == nullptr || !Window->IsValid())
	{
		return false;
	}

	DH_OnClosed = Window->Events.OnClosed.AddDynamic(
		[this](const SystemWindowEventTypes::FOnClosed&)->bool {
			GetSystem().RequestAppExit(EXIT_SUCCESS); return true; });

	auto& SystemInput{ System.GetInput() };
	Input = SystemInput.CreateContext(*Window);
	if (Input == nullptr || !Input->IsValid())
	{
		return false;
	}

	auto& SystemGraphics{ System.GetGraphics() };
	if (!SystemGraphics.SetRendererType(EGraphicsRendererType::Direct3D11))
	{
		return false;
	}

	Graphics = SystemGraphics.CreateContext(*Window);
	if (Graphics == nullptr || !Graphics->IsValid())
	{
		return false;
	}

	Window->Present();

	return true;
}

void FApplication::Terminate() noexcept
{
	Graphics.reset();
	Input.reset();
	DH_OnClosed.Release();
	Window.reset();
}

void FApplication::Tick(FTimeDuration DeltaTime)
{
	Window->Events.Process();
}

void FApplication::Render(FTimeDuration DeltaTime)
{
	Graphics->BeginScene();
	{

	}
	Graphics->EndScene();
}
