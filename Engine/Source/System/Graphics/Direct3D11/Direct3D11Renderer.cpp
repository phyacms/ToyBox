// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11Renderer.h"

#ifdef PLATFORM_WINDOWS

#include "Direct3D11SwapChain.h"

FDirect3D11Renderer::FDirect3D11Renderer(FGraphics& Graphics)
	: IGraphicsRenderer(Graphics)
{
}

FDirect3D11Renderer::~FDirect3D11Renderer() noexcept
{
}

std::unique_ptr<IGraphicsContext> FDirect3D11Renderer::CreateContext(FSystemWindow& OutputWindow)
{
	return std::make_unique<FDirect3D11SwapChain>(*this, OutputWindow);
}

#endif
