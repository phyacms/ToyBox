// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "System/Graphics/IGraphicsRenderer.h"

class FDirect3D11Renderer final
	: public IGraphicsRenderer
{
private:
	inline static constexpr UINT CreateDeviceFlags
	{
		D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT // Required by interoperable Direct2D.
#ifndef NDEBUG
		| D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG
#endif
	};
	inline static constexpr D3D_FEATURE_LEVEL MinimumFeatureLevel{ D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1 };

public:
	explicit FDirect3D11Renderer(FGraphics& Graphics);
	virtual ~FDirect3D11Renderer() noexcept { Terminate(); }

private:
	bool Initialize() noexcept;
	void Terminate() noexcept;

	static bool CheckRequirements() noexcept;
	bool CreateFactoryAndAdapter() noexcept;
	bool CreateDeviceAndContext() noexcept;
	bool CreateInteropFactories() noexcept;
	void QueryFeatures() noexcept;

public:
	virtual bool IsValid() const noexcept override final;

	virtual [[nodiscard]] std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow) & override final;

	inline IDXGIFactory1& GetFactory() const& noexcept { return *Factory.Get(); }
	inline IDXGIAdapter1& GetAdapter() const& noexcept { return *Adapter.Get(); }

	inline ID3D11Device& GetDevice() const& noexcept { return *Device.Get(); }
	inline ID3D11DeviceContext& GetDeviceContext() const& noexcept { return *Context.Get(); }

	inline ID2D1Factory& GetD2D1Factory() const& noexcept { return *D2D1Factory.Get(); }
	inline IDWriteFactory& GetDWriteFactory() const& noexcept { return *DWriteFactory.Get(); }

	inline bool AllowsTearing() const noexcept { return bAllowTearing; }

private:
	TComPtr<IDXGIFactory1> Factory;
	TComPtr<IDXGIAdapter1> Adapter;
	DXGI_ADAPTER_DESC1 AdapterDesc;

	TComPtr<ID3D11Device> Device;
	TComPtr<ID3D11DeviceContext> Context;

	TComPtr<ID2D1Factory> D2D1Factory;
	TComPtr<IDWriteFactory> DWriteFactory;

	bool bAllowTearing;
};

#endif
