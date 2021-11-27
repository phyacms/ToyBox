﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "Direct3D11Renderer.h"
#include "Direct2D1RenderTarget.h"
#include "System/Graphics/IGraphicsContext.h"

class FSystemWindow;

class FDirect3D11SwapChain final
	: public IGraphicsContext
{
public:
	FDirect3D11SwapChain(
		FDirect3D11Renderer& Renderer,
		FSystemWindow& OutputWindow);
	virtual ~FDirect3D11SwapChain() noexcept;

	FDirect3D11SwapChain(FDirect3D11SwapChain&&) noexcept = default;
	FDirect3D11SwapChain& operator=(FDirect3D11SwapChain&&) & noexcept = default;

private:
	bool Initialize() noexcept;
	void Terminate() noexcept;

	bool CreateSwapChain() noexcept;
	bool CreateResources() noexcept;
	void DestroyResources() noexcept;

	virtual bool IsValidImpl() const noexcept override final;
	virtual void ResizeBuffer(const FScreenSize& ClientAreaSize) override final;

	virtual void BeginScene(const FColor& ClearColor) const override final;
	virtual void EndScene() const override final;

public:
	virtual FScreenArea GetViewportArea() const noexcept override final;
	inline virtual ISurface2D& GetSurface() noexcept override final { return D2DRT; }

	inline FDirect3D11Renderer& GetRenderer() const noexcept { return *Renderer; }
	inline IDXGISwapChain& GetSwapChain() const& noexcept { return *SwapChain.Get(); }
	inline ID2D1RenderTarget& GetD2DRenderTarget() const& noexcept { return D2DRT.GetRenderTarget(); }

private:
	FDirect3D11Renderer* Renderer;

	TComPtr<IDXGISwapChain> SwapChain;
	UINT PresentFlags;

	TComPtr<ID3D11Resource> BackBuffer;
	TComPtr<ID3D11RenderTargetView> RenderTargetView;
	TComPtr<ID3D11DepthStencilState> DepthStencilState;
	TComPtr<ID3D11DepthStencilView> DepthStencilView;
	TComPtr<ID3D11RasterizerState> RasterizerState;
	TComPtr<ID3D11BlendState> BlendState;
	D3D11_VIEWPORT Viewport;
	D3D11_RECT ScissorRect;
	FDirect2D1RenderTarget D2DRT;
};

#endif
