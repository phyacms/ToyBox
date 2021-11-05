// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11Renderer.h"

#ifdef PLATFORM_WINDOWS

#include "Direct3D11SwapChain.h"

FDirect3D11Renderer::FDirect3D11Renderer(FGraphics& Graphics)
	: IGraphicsRenderer(Graphics)
	, Factory{}
	, Adapter{}
	, AdapterDesc{}
	, Device{}
	, Context{}
	, D2D1Factory{}
	, DWriteFactory{}
	, bAllowTearing{}
{
	if (!Initialize())
	{
		Terminate();
	}
}

bool FDirect3D11Renderer::Initialize() noexcept
{
	if (CheckRequirements()
		&& CreateFactoryAndAdapter()
		&& CreateDeviceAndContext()
		&& CreateInteropFactories())
	{
		QueryFeatures();
		return true;
	}
	return false;
}

void FDirect3D11Renderer::Terminate() noexcept
{
	Factory.Reset();
	Adapter.Reset();
	AdapterDesc = decltype(AdapterDesc){};

	Device.Reset();
	Context.Reset();

	D2D1Factory.Reset();
	DWriteFactory.Reset();

	bAllowTearing = false;
}

void FDirect3D11Renderer::QueryFeatures() noexcept
{
	BOOL AllowTearing{ FALSE };
	TComPtr<IDXGIFactory5> Factory5{};
	if (SUCCEEDED(::CreateDXGIFactory1(IID_PPV_ARGS(&Factory5))))
	{
		if (FAILED(Factory5->CheckFeatureSupport(
			DXGI_FEATURE::DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&AllowTearing,
			sizeof(AllowTearing))))
		{
			AllowTearing = FALSE;
		}
	}
	bAllowTearing = AllowTearing == TRUE;
}

bool FDirect3D11Renderer::CheckRequirements() noexcept
{
	return DirectX::XMVerifyCPUSupport();
}

bool FDirect3D11Renderer::CreateFactoryAndAdapter() noexcept
{
	if (FAILED(::CreateDXGIFactory1(IID_PPV_ARGS(&Factory))))
	{
		return false;
	}

	UINT Index{};
	TComPtr<IDXGIAdapter1> Adapter1{};
	SIZE_T MaxDedicatedVideoMemory{};
	while (Factory->EnumAdapters1(Index++, &Adapter1) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc{};
		if (SUCCEEDED(Adapter1->GetDesc1(&AdapterDesc))
			&& (AdapterDesc.Flags & DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE) == UINT{}
			&& SUCCEEDED(::D3D11CreateDevice(
				Adapter1.Get(),
				D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				CreateDeviceFlags,
				&MinimumFeatureLevel,
				UINT{ 1 },
				D3D11_SDK_VERSION,
				nullptr,
				nullptr,
				nullptr))
			&& AdapterDesc.DedicatedVideoMemory > MaxDedicatedVideoMemory)
		{
			Adapter = Adapter1;
			AdapterDesc = AdapterDesc;
			MaxDedicatedVideoMemory = AdapterDesc.DedicatedVideoMemory;
		}
	}
	if (Adapter == nullptr)
	{
		return false;
	}

	return true;
}

bool FDirect3D11Renderer::CreateDeviceAndContext() noexcept
{
	D3D_FEATURE_LEVEL FeatureLevel{};
	return SUCCEEDED(::D3D11CreateDevice(
		Adapter.Get(),
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		CreateDeviceFlags,
		&MinimumFeatureLevel,
		UINT{ 1 },
		D3D11_SDK_VERSION,
		&Device,
		&FeatureLevel,
		&Context))
		&& FeatureLevel == MinimumFeatureLevel;
}

bool FDirect3D11Renderer::CreateInteropFactories() noexcept
{
	// Interop D2D factory.
	if (FAILED(::D2D1CreateFactory(
		D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
		D2D1Factory.GetAddressOf())))
	{
		return false;
	}

	// Interop DirectWrite factory.
	if (FAILED(::DWriteCreateFactory(
		DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(DWriteFactory.GetAddressOf()))))
	{
		return false;
	}

	return true;
}

bool FDirect3D11Renderer::IsValid() const noexcept
{
	return !(false
		|| !Factory
		|| !Adapter
		|| !Device
		|| !Context
		|| !D2D1Factory
		|| !DWriteFactory);
}

std::unique_ptr<IGraphicsContext> FDirect3D11Renderer::CreateContext(FSystemWindow& OutputWindow)
{
	return std::make_unique<FDirect3D11SwapChain>(*this, OutputWindow);
}

#endif
