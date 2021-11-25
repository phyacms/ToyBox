﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11SwapChain.h"

#ifdef PLATFORM_WINDOWS

#include "System/Window/ISystemWindowProcedure.h"

FDirect3D11SwapChain::FDirect3D11SwapChain(
	FDirect3D11Renderer& Renderer,
	AObject<FSystemWindow>&& OutputWindow)
	: IGraphicsContext(Renderer, std::move(OutputWindow))
	, Renderer{ &Renderer }
	, SwapChain{}
	, PresentFlags{}
	, BackBuffer{}
	, RenderTargetView{}
	, DepthStencilState{}
	, DepthStencilView{}
	, RasterizerState{}
	, BlendState{}
	, Viewport{}
	, ScissorRect{}
	, D2DRenderTarget{}
{
	if (!Initialize())
	{
		Terminate();
	}
}

FDirect3D11SwapChain::~FDirect3D11SwapChain() noexcept
{
	Terminate();
}

bool FDirect3D11SwapChain::Initialize() noexcept
{
	return CreateSwapChain()
		&& CreateResources();
}

void FDirect3D11SwapChain::Terminate() noexcept
{
	DestroyResources();
	SwapChain.Reset();
}

bool FDirect3D11SwapChain::CreateSwapChain() noexcept
{
	auto& OutputWindow{ GetOutputWindow() };
	auto hWnd{ OutputWindow.InterfaceAs<WindowsPlatform::IWindowInterface>().GetHandle() };

	auto& Renderer{ GetRenderer() };
	auto& Factory{ Renderer.GetFactory() };
	auto& Device{ Renderer.GetDevice() };
	auto bAllowTearing{ Renderer.AllowsTearing() };

	// Disallows exclusive fullscreen mode and mode change by Alt+Enter.
	if (FAILED(Factory.MakeWindowAssociation(
		hWnd,
		DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER)))
	{
		return false;
	}

	// Cache present flags.
	PresentFlags = bAllowTearing
		? DXGI_PRESENT_ALLOW_TEARING
		: 0;

	// Create swap chain.
	DXGI_SWAP_CHAIN_DESC SwapChainDesc
	{
		.BufferDesc{
			.Width{},
			.Height{},
			.RefreshRate{ .Numerator{ 1 }, .Denominator{} },
			.Format{ DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM },
			.ScanlineOrdering{ DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED },
			.Scaling{ DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED } },
		.SampleDesc{ .Count{ 1 }, .Quality{} },
		.BufferUsage{ DXGI_USAGE_RENDER_TARGET_OUTPUT },
		.BufferCount{ 2 },
		.OutputWindow{ hWnd },
		.Windowed{ TRUE },
		.SwapEffect{ DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD },
		.Flags{
			bAllowTearing
			? DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
			: UINT{}},
	};
	if (FAILED(Factory.CreateSwapChain(&Device, &SwapChainDesc, &SwapChain)))
	{
		return false;
	}

	return true;
}

bool FDirect3D11SwapChain::CreateResources() noexcept
{
	// Swap chain back buffer resource.
	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	if (SwapChain == nullptr
		|| FAILED(SwapChain->GetDesc(&SwapChainDesc))
		|| FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer))))
	{
		return false;
	}

	auto& Renderer{ GetRenderer() };
	auto& Device{ Renderer.GetDevice() };

	// Swap chain render target view.
	if (FAILED(Device.CreateRenderTargetView(
		BackBuffer.Get(),
		nullptr,
		RenderTargetView.GetAddressOf())))
	{
		return false;
	}

	// Depth-stencil state.
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{
		.DepthEnable{ TRUE },
		.DepthWriteMask{ D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL },
		.DepthFunc{ D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS },
		.StencilEnable{},
		.StencilReadMask{},
		.StencilWriteMask{},
		.FrontFace{},
		.BackFace{} };
	if (FAILED(Device.CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState)))
	{
		return false;
	}

	TComPtr<ID3D11Texture2D> DepthStencilBuffer;
	D3D11_TEXTURE2D_DESC DepthStencilBufferDesc{
		.Width{ SwapChainDesc.BufferDesc.Width },
		.Height{ SwapChainDesc.BufferDesc.Height },
		.MipLevels{ 1 },
		.ArraySize{ 1 },
		.Format{ DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT },
		.SampleDesc{ SwapChainDesc.SampleDesc },
		.Usage{ D3D11_USAGE::D3D11_USAGE_DEFAULT },
		.BindFlags{ D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL },
		.CPUAccessFlags{},
		.MiscFlags{} };
	if (FAILED(Device.CreateTexture2D(&DepthStencilBufferDesc, nullptr, &DepthStencilBuffer)))
	{
		return false;
	}

	// Depth-stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc{
		.Format{ DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT },
		.ViewDimension{ D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D },
		.Flags{},
		.Texture2D{ .MipSlice{} } };
	if (FAILED(Device.CreateDepthStencilView(
		DepthStencilBuffer.Get(),
		&DepthStencilViewDesc,
		DepthStencilView.GetAddressOf())))
	{
		return false;
	}

	// Rasterizer state.
	D3D11_RASTERIZER_DESC RasterizerDesc{
		.FillMode{ D3D11_FILL_MODE::D3D11_FILL_SOLID },
		.CullMode{ D3D11_CULL_MODE::D3D11_CULL_BACK },
		.FrontCounterClockwise{ TRUE },
		.DepthBias{ D3D11_DEFAULT_DEPTH_BIAS },
		.DepthBiasClamp{ D3D11_DEFAULT_DEPTH_BIAS_CLAMP },
		.SlopeScaledDepthBias{ D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS },
		.DepthClipEnable{ TRUE },
		.ScissorEnable{ FALSE },
		.MultisampleEnable{ FALSE },
		.AntialiasedLineEnable{ FALSE } };
	if (FAILED(Device.CreateRasterizerState(&RasterizerDesc, &RasterizerState)))
	{
		return false;
	}

	// Blend state.
	D3D11_BLEND_DESC BlendDesc{
		.AlphaToCoverageEnable{ FALSE },
		.IndependentBlendEnable{ FALSE },
		.RenderTarget{ {
			.BlendEnable{ TRUE },
			.SrcBlend{ D3D11_BLEND::D3D11_BLEND_SRC_ALPHA },
			.DestBlend{ D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA },
			.BlendOp{ D3D11_BLEND_OP::D3D11_BLEND_OP_ADD },
			.SrcBlendAlpha{ D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA },
			.DestBlendAlpha{ D3D11_BLEND::D3D11_BLEND_ZERO },
			.BlendOpAlpha{ D3D11_BLEND_OP::D3D11_BLEND_OP_ADD },
			.RenderTargetWriteMask{ D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL } }, } };
	if (FAILED(Device.CreateBlendState(&BlendDesc, &BlendState)))
	{
		return false;
	}

	// Viewport setup.
	Viewport = {
		.TopLeftX{},
		.TopLeftY{},
		.Width{ static_cast<FLOAT>(SwapChainDesc.BufferDesc.Width) },
		.Height{ static_cast<FLOAT>(SwapChainDesc.BufferDesc.Height) },
		.MinDepth{},
		.MaxDepth{ 1.0f },
	};

	// Scissor rect setup.
	ScissorRect = {
		.left{},
		.top{},
		.right{ static_cast<LONG>(SwapChainDesc.BufferDesc.Width) },
		.bottom{ static_cast<LONG>(SwapChainDesc.BufferDesc.Height) } };

	// Interoperable Direct2D render target.
	TComPtr<IDXGISurface> Surface{};
	if (FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&Surface))))
	{
		return false;
	}

	const D2D1_RENDER_TARGET_PROPERTIES& RenderTargetProperties = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED),
		96.0f,
		96.0f,
		D2D1_RENDER_TARGET_USAGE::D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL::D2D1_FEATURE_LEVEL_DEFAULT);

	if (FAILED(Renderer.GetD2D1Factory().CreateDxgiSurfaceRenderTarget(
		Surface.Get(),
		&RenderTargetProperties,
		D2DRenderTarget.GetAddressOf())))
	{
		return false;
	}

	return true;
}

void FDirect3D11SwapChain::DestroyResources() noexcept
{
	BackBuffer.Reset();
	RenderTargetView.Reset();
	DepthStencilState.Reset();
	DepthStencilView.Reset();
	RasterizerState.Reset();
	BlendState.Reset();
	Viewport = {};
	ScissorRect = {};
	D2DRenderTarget.Reset();
}

bool FDirect3D11SwapChain::IsValidImpl() const noexcept
{
	return !(false
		|| !SwapChain
		|| !BackBuffer
		|| !RenderTargetView
		|| !DepthStencilState
		|| !DepthStencilView
		|| !RasterizerState
		|| !BlendState
		|| !D2DRenderTarget);
}

void FDirect3D11SwapChain::ResizeBuffer(const FScreenSize& ClientAreaSize)
{
	DestroyResources();
	if (SwapChain != nullptr)
	{
		DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
		if (SUCCEEDED(SwapChain->GetDesc(&SwapChainDesc)))
		{
			if (SUCCEEDED(SwapChain->ResizeBuffers(
				0,
				ClientAreaSize.X<UINT>(),
				ClientAreaSize.Y<UINT>(),
				SwapChainDesc.BufferDesc.Format,
				SwapChainDesc.Flags)))
			{
				CreateResources();
			}
		}
	}
}

void FDirect3D11SwapChain::BeginScene(const FColor& ClearColor) const
{
	static constexpr FLOAT BlendFactor[4]{};
	static constexpr UINT SampleMask{ 0xFFFFFFFF };

	auto& Context = GetRenderer().GetDeviceContext();

	Context.ClearRenderTargetView(RenderTargetView.Get(), ClearColor.GetPtr());
	Context.ClearDepthStencilView(
		DepthStencilView.Get(),
		D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	Context.RSSetState(RasterizerState.Get());
	Context.RSSetViewports(1, &Viewport);
	Context.RSSetScissorRects(1, &ScissorRect);
	Context.OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Context.OMSetBlendState(BlendState.Get(), BlendFactor, SampleMask);
	Context.OMSetDepthStencilState(DepthStencilState.Get(), 1);

	D2DRenderTarget->BeginDraw();
}

void FDirect3D11SwapChain::EndScene() const
{
	D2DRenderTarget->EndDraw();
	SwapChain->Present(0, PresentFlags);
}

#endif