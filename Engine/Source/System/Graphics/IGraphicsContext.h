// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Object.h"
#include "Type/TimePoint.h"
#include "System/Window/SystemWindow.h"
#include "IGraphicsRenderer.h"

class IGraphicsContext
{
public:
	IGraphicsContext(
		AObject<IGraphicsRenderer>&& Renderer,
		AObject<FSystemWindow>&& OutputWindow)
		: Renderer{ std::move(Renderer) }
		, OutputWindow{ std::move(OutputWindow) } {}
	virtual ~IGraphicsContext() noexcept = default;

	IGraphicsContext(const IGraphicsContext&) = delete;
	IGraphicsContext& operator=(const IGraphicsContext&) = delete;
	IGraphicsContext(IGraphicsContext&&) noexcept = default;
	IGraphicsContext& operator=(IGraphicsContext&&) noexcept = default;

public:
	bool IsValid() const noexcept { return OutputWindow.IsValid() && IsValidImpl(); }
	virtual void Render(FTimeDuration DeltaTime) = 0;

protected:
	inline IGraphicsRenderer& GetRenderer() const noexcept { return *Renderer.GetAddress(); }
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<IGraphicsRenderer, T>>>
	inline T& GetRendererAs() const noexcept { return dynamic_cast<T&>(GetRenderer()); }
	inline FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;

private:
	AObject<IGraphicsRenderer> Renderer;
	AObject<FSystemWindow> OutputWindow;
};
