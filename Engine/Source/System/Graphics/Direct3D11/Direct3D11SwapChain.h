// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "Direct3D11Renderer.h"
#include "System/Graphics/IGraphicsContext.h"

class FSystemWindow;

class FDirect3D11SwapChain final
	: public IGraphicsContext
{
public:
	FDirect3D11SwapChain(
		FDirect3D11Renderer& Renderer,
		AObject<FSystemWindow>&& OutputWindow);
	virtual ~FDirect3D11SwapChain() noexcept;

	FDirect3D11SwapChain(FDirect3D11SwapChain&&) noexcept = default;
	FDirect3D11SwapChain& operator=(FDirect3D11SwapChain&&) noexcept = default;

private:
	bool Initialize() noexcept;
	void Terminate() noexcept;

	bool CreateSwapChain() noexcept;
	bool CreateResources() noexcept;
	void DestroyResources() noexcept;
	virtual void ResizeBuffer(std::uint32_t Width, std::uint32_t Height) override final;

	virtual bool IsValidImpl() const noexcept override final;
	virtual void Render(FTimeDuration DeltaTime) override final;

	void BeginScene(const FColorRGBA& ClearColor = IGraphicsContext::DefaultClearColor) const;
	void EndScene() const;

	inline FDirect3D11Renderer& GetRenderer() const noexcept { return IGraphicsContext::GetRendererAs<FDirect3D11Renderer>(); }

private:
	TComPtr<IDXGISwapChain> SwapChain;
	UINT PresentFlags;

	TComPtr<ID3D11Resource> BackBuffer{};
	TComPtr<ID3D11RenderTargetView> RenderTargetView{};
	TComPtr<ID3D11DepthStencilState> DepthStencilState{};
	TComPtr<ID3D11DepthStencilView> DepthStencilView{};
	TComPtr<ID3D11RasterizerState> RasterizerState{};
	TComPtr<ID3D11BlendState> BlendState{};
	D3D11_VIEWPORT Viewport{};
	D3D11_RECT ScissorRect{};
	TComPtr<ID2D1RenderTarget> D2DRenderTarget{};
};

#endif
