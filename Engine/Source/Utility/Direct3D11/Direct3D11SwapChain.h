// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/IGraphicsContext.h"
#include "Direct3D11.h"
#include "Direct3D11Renderer.h"
#include "Direct3D11Texture2D.h"

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
	virtual void UpdateViewport(const FScreenArea& ViewportArea) override final;
	virtual void UpdateProjection(const FProjection& Projection) override final;

	virtual void BeginScene(const FColor& BackgroundColor, const FColor& ClearColor) const override final;
	virtual void EndScene() const override final;

	void FlushD2D() const;

public:
	inline virtual const Affine::Matrix4x4f& GetProjectionMatrix() const noexcept override final { return CachedProjectionMatrix; }

	inline FDirect3D11Renderer& GetRenderer() const noexcept { return *Renderer; }
	inline IDXGISwapChain& GetSwapChain() const& noexcept { return *SwapChain.Get(); }

	inline ID2D1RenderTarget& GetD2DRenderTarget() const& noexcept { return *D2D.RenderTarget.Get(); }
	ID2D1SolidColorBrush& GetD2DBrush(const FColor& Color) const;

	virtual void DrawLine(
		const FScreenLocation& Begin,
		const FScreenLocation& End,
		const FColor& Color = {},
		float Width = 1.0f) override final;
	virtual void DrawRect(
		const FScreenArea& Rect,
		const FColor& Color = {},
		float Width = 1.0f) override final;

private:
	FDirect3D11Renderer* Renderer;

	TComPtr<IDXGISwapChain> SwapChain;
	UINT PresentFlags;

	FDirect3D11Texture2D BackBuffer;
	TComPtr<ID3D11RenderTargetView> RenderTargetView;
	TComPtr<ID3D11DepthStencilState> DepthStencilState;
	TComPtr<ID3D11DepthStencilView> DepthStencilView;
	TComPtr<ID3D11RasterizerState> RasterizerState;
	TComPtr<ID3D11BlendState> BlendState;
	D3D11_VIEWPORT Viewport;
	D3D11_RECT ScissorRect;
	struct FInteropDirect2D final
	{
		TComPtr<ID2D1RenderTarget> RenderTarget{};
		mutable std::unordered_map<FColorCode::ValueType, TComPtr<ID2D1SolidColorBrush>> Brushes{};
	}
	D2D;

	Affine::Matrix4x4f CachedProjectionMatrix;
};

#endif
