// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "System/Graphics/IGraphicsContext.h"

class FSystemWindow;
class FDirect3D11Renderer;

class FDirect3D11SwapChain final
	: public IGraphicsContext
{
public:
	FDirect3D11SwapChain(FDirect3D11Renderer& Renderer, FSystemWindow& OutputWindow);
	virtual ~FDirect3D11SwapChain() noexcept;

	FDirect3D11SwapChain(FDirect3D11SwapChain&&) noexcept = default;
	FDirect3D11SwapChain& operator=(FDirect3D11SwapChain&&) noexcept = default;

private:
	virtual bool IsValidImpl() const noexcept override final;
	virtual void Render(FTimeDuration DeltaTime) const override final;
};

#endif
