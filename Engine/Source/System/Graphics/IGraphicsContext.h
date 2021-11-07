// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Object.h"
#include "Type/TimePoint.h"
#include "Type/Color.h"
#include "System/Window/SystemWindow.h"
#include "IGraphicsRenderer.h"

class IGraphicsContext
{
public:
	static constexpr FColorRGBA DefaultClearColor{ 0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f };

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
	bool IsValid() const noexcept { return OutputWindow.IsValid() && IsValidImpl(); }
	virtual void Render(FTimeDuration DeltaTime) = 0;

protected:
	inline IGraphicsRenderer& GetRenderer() const noexcept { return *Renderer.GetAddress(); }
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<IGraphicsRenderer, T>>>
	inline T& GetRendererAs() const noexcept { return dynamic_cast<T&>(GetRenderer()); }
	inline FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;
	virtual void ResizeBuffer(std::uint32_t Width, std::uint32_t Height) = 0;

private:
	AObject<IGraphicsRenderer> Renderer;
	AObject<FSystemWindow> OutputWindow;
	ADelegateHandle DH_OnResized;
};
