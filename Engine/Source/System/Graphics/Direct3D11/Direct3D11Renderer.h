// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "System/Graphics/IGraphicsRenderer.h"

class FDirect3D11Renderer final
	: public IGraphicsRenderer
{
public:
	explicit FDirect3D11Renderer(FGraphics& Graphics);
	virtual ~FDirect3D11Renderer() noexcept;

public:
	virtual std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow) override final;
};

#endif
