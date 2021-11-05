﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

class FSystemWindow;
class FGraphics;
class IGraphicsContext;

enum class EGraphicsRendererType : std::size_t
{
	Unknown,
	Direct3D11, // @NOTE: Available in Windows platform only.
};

class IGraphicsRenderer
{
public:
	explicit IGraphicsRenderer(FGraphics& Graphics) : Graphics{ &Graphics } {}
	virtual ~IGraphicsRenderer() noexcept = default;

	IGraphicsRenderer(const IGraphicsRenderer&) = delete;
	IGraphicsRenderer& operator=(const IGraphicsRenderer&) = delete;
	IGraphicsRenderer(IGraphicsRenderer&&) noexcept = delete;
	IGraphicsRenderer& operator=(IGraphicsRenderer&&) noexcept = delete;

public:
	virtual std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow) = 0;

protected:
	inline FGraphics& GetGraphics() const noexcept { return *Graphics; }

private:
	FGraphics* Graphics;
};
