// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

class FSystem;
class FSystemWindow;
class IGraphicsRenderer;
class IGraphicsContext;
enum class EGraphicsRendererType : std::size_t;

class FGraphics final
{
public:
	explicit FGraphics(FSystem& System);
	~FGraphics() noexcept;

	FGraphics(const FGraphics&) = delete;
	FGraphics& operator=(const FGraphics&) = delete;
	FGraphics(FGraphics&&) noexcept = delete;
	FGraphics& operator=(FGraphics&&) noexcept = delete;

public:
	bool SetRendererType(EGraphicsRendererType RendererType) noexcept;
	std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow);

private:
	FSystem* System;
	std::unique_ptr<IGraphicsRenderer> Renderer;
};
