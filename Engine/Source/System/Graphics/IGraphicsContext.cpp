// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IGraphicsContext.h"

IGraphicsContext::IGraphicsContext(
	AObject<IGraphicsRenderer>&& Renderer,
	AObject<FSystemWindow>&& OutputWindow)
	: Renderer{ std::move(Renderer) }
	, OutputWindow{ std::move(OutputWindow) }
	, DH_OnResized{}
{
	if (this->OutputWindow.IsValid())
	{
		using namespace SystemWindowEventTypes;

		DH_OnResized = this->OutputWindow->Events.OnResized.AddDynamic(
			[this](const FOnResized& EventArgs)->bool {
				ResizeBuffer(EventArgs.ClientAreaSize); return false; });
	}
}

IGraphicsContext::~IGraphicsContext() noexcept
{
	DH_OnResized.Release();
	OutputWindow.Release();
	Renderer.Release();
}
