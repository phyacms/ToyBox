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
	ClearRenderCommands();
	DH_OnResized.Release();
	OutputWindow.Release();
	Renderer.Release();
}

void IGraphicsContext::AddCommand(FRenderCommand&& Command)
{
	CommandQueue.emplace(std::move(Command));
}

void IGraphicsContext::ExecuteCommands(FTimeDuration DeltaTime)
{
	struct FScene final
	{
		const IGraphicsContext* Context{};
		FScene(const IGraphicsContext& Context)
			: Context{ &Context } { Context.BeginScene(Context.ClearColor); }
		~FScene() noexcept { Context->EndScene(); }
	}
	Scene{ *this };

	while (!CommandQueue.empty())
	{
		CommandQueue.front().Execute(DeltaTime);
		CommandQueue.pop();
	}
}
