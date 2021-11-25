// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "IGraphicsRenderer.h"

class FSystem;
class FSystemWindow;
class IGraphicsContext;

class FGraphics final
{
public:
	explicit FGraphics(FSystem& System);
	~FGraphics() noexcept;

	FGraphics(const FGraphics&) = delete;
	FGraphics& operator=(const FGraphics&) & = delete;
	FGraphics(FGraphics&&) noexcept = delete;
	FGraphics& operator=(FGraphics&&) & noexcept = delete;

public:
	template<typename T, typename... Ts, typename = std::enable_if_t<std::is_base_of_v<IGraphicsRenderer, T>>>
	inline bool CreateRenderer(Ts&&... Params) noexcept
	{
		Renderer = std::make_unique<T>(*this, std::forward<Ts>(Params)...);
		if (Renderer == nullptr || !Renderer->IsValid())
		{
			Renderer.reset();
			return false;
		}

		return true;
	}
	[[nodiscard]] std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow) &;

private:
	FSystem* System;
	std::unique_ptr<IGraphicsRenderer> Renderer;
};
