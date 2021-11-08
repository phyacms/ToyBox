// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>
#include <System/Window/SystemWindow.h>
#include <System/Graphics/Graphics.h>
#include <System/Graphics/IGraphicsRenderer.h>

FApplication::FApplication(FSystem& System)
	: IApplication(System)
	, Window{}
	, DH_OnClosed{}
	, GraphicsContext{}
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
		[this]()->bool { GetSystem().RequestAppExit(EXIT_SUCCESS); return true; });

	auto& Graphics = System.GetGraphics();
	if (!Graphics.SetRendererType(EGraphicsRendererType::Direct3D11))
	{
		return false;
	}

	GraphicsContext = Graphics.CreateContext(*Window);
	if (GraphicsContext == nullptr || !GraphicsContext->IsValid())
	{
		return false;
	}

	Window->Present();

	return true;
}

void FApplication::Terminate() noexcept
{
	GraphicsContext.reset();
	DH_OnClosed.Release();
	Window.reset();
}

void FApplication::Tick(FTimeDuration DeltaTime)
{
	Window->Events.Process();
}

void FApplication::Render(FTimeDuration DeltaTime)
{
	GraphicsContext->BeginScene();
	{
		// @TODO: Render.
	}
	GraphicsContext->EndScene();
}
