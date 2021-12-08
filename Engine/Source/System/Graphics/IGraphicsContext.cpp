// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IGraphicsContext.h"
#include "IGraphicsRenderer.h"
#include "System/Window/SystemWindow.h"

IGraphicsContext::IGraphicsContext(
	IGraphicsRenderer& Renderer,
	FSystemWindow& OutputWindow)
	: TObject<IGraphicsContext>(*this)
	, Renderer{ Renderer }
	, OutputWindow{ OutputWindow }
	, DH_OnResized{}
	, Viewport{}
	, Projection{ FPerspectiveProjectionFoV{} }
	, BackgroundColor{ ColorCodes::Black }
	, ClearColor{ ColorCodes::CornflowerBlue }
	, OnViewportChanged{}
{
	if (this->OutputWindow.IsValid())
	{
		DH_OnResized = this->OutputWindow->Events.OnResized.AddDynamic(
			[this](const FOnResized& EventArgs)->bool {
				ResizeBuffer(EventArgs.ClientAreaSize);
				UpdateViewport();
				return false; });
	}
}

IGraphicsContext::~IGraphicsContext() noexcept
{
	DH_OnResized.Release();
	OutputWindow.Release();
	Renderer.Release();
}

bool IGraphicsContext::IsValid() const noexcept
{
	return Renderer.IsValid()
		&& Renderer->IsValid()
		&& OutputWindow.IsValid()
		&& IsValidImpl();
}

void IGraphicsContext::SetViewportDimension(UDim Dimension)
{
	Viewport.SetDimension(std::move(Dimension), GetViewportTarget());
	UpdateViewport();
}

void IGraphicsContext::SetViewportAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio)
{
	Viewport.SetAspectRatio(MinimumAspectRatio, MaximumAspectRatio);
	UpdateViewport();
}

void IGraphicsContext::SetProjection(FProjection Projection)
{
	this->Projection = std::move(Projection);
	UpdateProjection();
}

void IGraphicsContext::UpdateViewport()
{
	Viewport.UpdateArea(GetViewportTarget());
	UpdateViewport(GetViewportArea());
	UpdateProjection();
	OnViewportChanged.Broadcast(GetViewportArea());
}

FScreenArea IGraphicsContext::GetViewportTarget() const noexcept
{
	return { .Location{}, .Size{ GetOutputWindow().GetClientArea().Size } };
}
