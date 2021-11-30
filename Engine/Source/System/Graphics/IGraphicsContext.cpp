// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IGraphicsContext.h"
#include "System/Window/SystemWindow.h"

IGraphicsContext::IGraphicsContext(
	IGraphicsRenderer& Renderer,
	FSystemWindow& OutputWindow,
	FColor ClearColor)
	: TObject<IGraphicsContext>(*this)
	, Renderer{ Renderer }
	, OutputWindow{ OutputWindow }
	, DH_OnResized{}
	, ClearColor{ ClearColor }
	, OnViewportAreaChanged{}
{
	if (this->OutputWindow.IsValid())
	{
		DH_OnResized = this->OutputWindow->Events.OnResized.AddDynamic(
			[this](const FOnResized& EventArgs)->bool {
				ResizeBuffer(EventArgs.ClientAreaSize);
				OnViewportAreaChanged.Broadcast(GetViewportArea());
				return false; });
	}
}

IGraphicsContext::~IGraphicsContext() noexcept
{
	DH_OnResized.Release();
	OutputWindow.Release();
	Renderer.Release();
}
