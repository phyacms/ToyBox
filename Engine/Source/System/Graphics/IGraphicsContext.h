// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/Time.h"
#include "Type/Color.h"
#include "Type/Delegate/Delegate.h"
#include "Type/Delegate/MulticastDelegate.h"
#include "System/Window/ScreenSpace.h"
#include "IGraphicsRenderer.h"
#include "ISurface2D.h"

class FSystemWindow;
using FRenderCommand = TDelegate<void(const FTimeDuration&)>;

class IGraphicsContext
	: public TObject<IGraphicsContext>
{
public:
	IGraphicsContext(
		IGraphicsRenderer& Renderer,
		FSystemWindow& OutputWindow,
		FColor ClearColor = ColorCodes::CornflowerBlue);
	virtual ~IGraphicsContext() noexcept;

	IGraphicsContext(const IGraphicsContext&) = delete;
	IGraphicsContext& operator=(const IGraphicsContext&) & = delete;
	IGraphicsContext(IGraphicsContext&&) noexcept = default;
	IGraphicsContext& operator=(IGraphicsContext&&) & noexcept = default;

public:
	inline bool IsValid() const noexcept { return Renderer.IsValid() && OutputWindow.IsValid() && IsValidImpl(); }

	inline const FColor& GetClearColor() const noexcept { return ClearColor; }
	virtual FScreenArea GetViewportArea() const noexcept = 0;

	inline void SetClearColor(FColor ClearColor) noexcept { this->ClearColor = std::move(ClearColor); }

	void AddCommand(FRenderCommand&& Command);
	void ExecuteCommands(const FTimeDuration& DeltaTime);

	virtual ISurface2D& GetSurface() noexcept = 0;

protected:
	inline FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;
	virtual void ResizeBuffer(const FScreenSize& ClientAreaSize) = 0;

	virtual void BeginScene(const FColor& ClearColor) const = 0;
	virtual void EndScene() const = 0;

	void ClearRenderCommands() noexcept { CommandQueue = {}; }

private:
	AObject<IGraphicsRenderer> Renderer;
	AObject<FSystemWindow> OutputWindow;
	ADelegateHandle DH_OnResized;
	FColor ClearColor;

	std::queue<FRenderCommand> CommandQueue;

public:
	using FOnViewportAreaChanged = TMulticastDelegate<void(const FScreenArea&)>;
	FOnViewportAreaChanged OnViewportAreaChanged;
};
