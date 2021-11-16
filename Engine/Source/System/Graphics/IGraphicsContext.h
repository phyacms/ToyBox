// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/Color.h"
#include "System/Window/ScreenSpace.h"
#include "System/Window/SystemWindow.h"
#include "IGraphicsRenderer.h"

class IGraphicsContext
{
public:
	IGraphicsContext(
		AObject<IGraphicsRenderer>&& Renderer,
		AObject<FSystemWindow>&& OutputWindow);
	virtual ~IGraphicsContext() noexcept;

	IGraphicsContext(const IGraphicsContext&) = delete;
	IGraphicsContext& operator=(const IGraphicsContext&) & = delete;
	IGraphicsContext(IGraphicsContext&&) noexcept = default;
	IGraphicsContext& operator=(IGraphicsContext&&) & noexcept = default;

public:
	inline bool IsValid() const noexcept { return Renderer.IsValid() && OutputWindow.IsValid() && IsValidImpl(); }

	virtual void BeginScene(const FColor& ClearColor = FColor{ sRGB::Color::CornflowerBlue }) const = 0;
	virtual void EndScene() const = 0;

protected:
	inline FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;
	virtual void ResizeBuffer(const FScreenSize& ClientAreaSize) = 0;

private:
	AObject<IGraphicsRenderer> Renderer;
	AObject<FSystemWindow> OutputWindow;
	ADelegateHandle DH_OnResized;
};
