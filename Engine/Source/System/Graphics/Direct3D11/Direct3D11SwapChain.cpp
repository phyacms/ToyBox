// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11SwapChain.h"

#ifdef PLATFORM_WINDOWS

#include "System/Window/ISystemWindowProcedure.h"

FDirect3D11SwapChain::FDirect3D11SwapChain(
	FDirect3D11Renderer& Renderer,
	AObject<FSystemWindow>&& OutputWindow)
	: IGraphicsContext(Renderer, std::move(OutputWindow))
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
		: UINT{ 0 };

	// Create swap chain.
	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	SwapChainDesc.BufferDesc.Width = UINT{};
	SwapChainDesc.BufferDesc.Height = UINT{};
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = UINT{ 1 };
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = UINT{ 0 };
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.SampleDesc.Count = UINT{ 1 };
	SwapChainDesc.SampleDesc.Quality = UINT{ 0 };
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = UINT{ 2 };
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = bAllowTearing
		? DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
		: UINT{ 0 };

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
		|| FAILED(SwapChain->GetBuffer(UINT{}, IID_PPV_ARGS(&BackBuffer))))
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
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{};
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	if (FAILED(Device.CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState)))
	{
		return false;
	}

	TComPtr<ID3D11Texture2D> DepthStencilBuffer;
	D3D11_TEXTURE2D_DESC DepthStencilBufferDesc{};
	DepthStencilBufferDesc.Width = SwapChainDesc.BufferDesc.Width;
	DepthStencilBufferDesc.Height = SwapChainDesc.BufferDesc.Height;
	DepthStencilBufferDesc.MipLevels = UINT{ 1 };
	DepthStencilBufferDesc.ArraySize = UINT{ 1 };
	DepthStencilBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	DepthStencilBufferDesc.SampleDesc.Count = UINT{ 1 };
	DepthStencilBufferDesc.SampleDesc.Quality = UINT{};
	DepthStencilBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	DepthStencilBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	DepthStencilBufferDesc.CPUAccessFlags = UINT{};
	DepthStencilBufferDesc.MiscFlags = UINT{};
	if (FAILED(Device.CreateTexture2D(&DepthStencilBufferDesc, nullptr, &DepthStencilBuffer)))
	{
		return false;
	}

	// Depth-stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc{};
	DepthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = UINT{};
	if (FAILED(Device.CreateDepthStencilView(
		DepthStencilBuffer.Get(),
		&DepthStencilViewDesc,
		DepthStencilView.GetAddressOf())))
	{
		return false;
	}

	// Rasterizer state.
	D3D11_RASTERIZER_DESC RasterizerDesc{};
	RasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = TRUE;
	RasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	RasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	RasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;
	if (FAILED(Device.CreateRasterizerState(&RasterizerDesc, &RasterizerState)))
	{
		return false;
	}

	// Blend state.
	D3D11_BLEND_DESC BlendDesc{};
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(Device.CreateBlendState(&BlendDesc, &BlendState)))
	{
		return false;
	}

	// Viewport setup.
	Viewport.Width = static_cast<FLOAT>(SwapChainDesc.BufferDesc.Width);
	Viewport.Height = static_cast<FLOAT>(SwapChainDesc.BufferDesc.Height);
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	// Scissor rect setup.
	ScissorRect.left = 0;
	ScissorRect.top = 0;
	ScissorRect.right = static_cast<LONG>(SwapChainDesc.BufferDesc.Width);
	ScissorRect.bottom = static_cast<LONG>(SwapChainDesc.BufferDesc.Height);

	// Interoperable Direct2D render target.
	TComPtr<IDXGISurface> Surface;
	if (FAILED(SwapChain->GetBuffer(0u, IID_PPV_ARGS(&Surface))))
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
	Viewport = decltype(Viewport){};
	ScissorRect = decltype(ScissorRect){};
	D2DRenderTarget.Reset();
}

bool FDirect3D11SwapChain::IsValidImpl() const noexcept
{
	return true;
}

void FDirect3D11SwapChain::Render(FTimeDuration DeltaTime)
{
	BeginScene();
	{
		// @TODO: Render commands.
	}
	EndScene();
}

void FDirect3D11SwapChain::BeginScene() const
{
	static constexpr FLOAT ClearColor[4]{ 0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f };
	static constexpr FLOAT BlendFactor[4]{};
	static constexpr UINT SampleMask{ 0xFFFFFFFF };

	auto& Context = GetRenderer().GetDeviceContext();

	Context.ClearRenderTargetView(RenderTargetView.Get(), ClearColor);
	Context.ClearDepthStencilView(
		DepthStencilView.Get(),
		D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL,
		FLOAT{ 1.0f },
		UINT8{ 0 });

	Context.RSSetState(RasterizerState.Get());
	Context.RSSetViewports(1u, &Viewport);
	Context.RSSetScissorRects(UINT{ 1 }, &ScissorRect);
	Context.OMSetRenderTargets(UINT{ 1 }, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Context.OMSetBlendState(BlendState.Get(), BlendFactor, SampleMask);
	Context.OMSetDepthStencilState(DepthStencilState.Get(), UINT{ 1 });

	D2DRenderTarget->BeginDraw();
}

void FDirect3D11SwapChain::EndScene() const
{
	D2DRenderTarget->EndDraw();
	SwapChain->Present(UINT{}, PresentFlags);
}

#endif
