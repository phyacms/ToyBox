// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>
#include <System/Window/SystemWindow.h>
#include <System/Input/Input.h>
#include <System/Input/InputContext.h>
#include <System/Graphics/Graphics.h>
#include <System/Graphics/IGraphicsContext.h>
#include <Player/HUD/HUD.h>
#include <Utility/Direct3D11/Direct3D11Renderer.h>

FApplication::FApplication(FSystem& System)
	: IApplication(System)
	, IInputActionController()
	, Window{}
	, Input{}
	, Graphics{}
	, HUD{}
	, DH_OnClosed{}
	, IC_Application{}
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
		[this](const FOnClosed&)->bool {
			GetSystem().RequestAppExit(EXIT_SUCCESS); return true; });

	auto& SystemInput{ System.GetInput() };
	Input = SystemInput.CreateContext(*Window);
	if (Input == nullptr || !Input->IsValid())
	{
		return false;
	}

	auto& SystemGraphics{ System.GetGraphics() };
	if (!SystemGraphics.CreateRenderer<FDirect3D11Renderer>())
	{
		return false;
	}

	Graphics = SystemGraphics.CreateContext(*Window);
	if (Graphics == nullptr || !Graphics->IsValid())
	{
		return false;
	}

	HUD = std::make_unique<FHUD>(*Input, *Graphics);
	if (HUD == nullptr || !HUD->IsValid())
	{
		return false;
	}

	Window->Present();
	IC_Application = Input->BindInputController(*this);

	return true;
}

void FApplication::Terminate() noexcept
{
	IC_Application.Release();
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
	auto Scene{ Graphics->CreateScene() };

	HUD->Render(DeltaTime);
}

void FApplication::BindInputActions(FInputActionBindings& Actions)
{
	Actions += FInputAction{
		.Chord{ FInputChord{}
			.AddModifier(EKeyboardKey::Alt)
			.SetTrigger(FInputCodeTrigger{ .InputCode{ EKeyboardKey::F4 }, .Event{ ESwitchEvent::Pressed } }) },
		.Callback{ [this](const FInputContext&)->bool { Window->Close(); return true; } } };
}
