// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11SwapChain.h"

#ifdef PLATFORM_WINDOWS

FDirect3D11SwapChain::FDirect3D11SwapChain(FDirect3D11Renderer& Renderer, FSystemWindow& OutputWindow)
	: IGraphicsContext(OutputWindow)
{
}

FDirect3D11SwapChain::~FDirect3D11SwapChain() noexcept
{
}

bool FDirect3D11SwapChain::IsValidImpl() const noexcept
{
	return true;
}

void FDirect3D11SwapChain::Render(FTimeDuration DeltaTime) const
{
}

#endif
