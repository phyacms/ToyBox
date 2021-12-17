// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "Type/Media/ImageFrame.h"

class FDirect3D11Texture2D
{
public:
	static FDirect3D11Texture2D GetSwapChainBackBuffer(IDXGISwapChain& SwapChain) noexcept;

public:
	FDirect3D11Texture2D() : Texture{}, Desc{} {}
	FDirect3D11Texture2D(
		ID3D11Device& Device,
		const FImageFrame& Frame,
		UINT BindFlags,
		D3D11_USAGE Usage,
		UINT CPUAccessFlags,
		UINT MiscFlags);
	FDirect3D11Texture2D(const FDirect3D11Texture2D&) = default;
	FDirect3D11Texture2D& operator=(const FDirect3D11Texture2D&) & = default;
	FDirect3D11Texture2D(FDirect3D11Texture2D&&) noexcept = default;
	FDirect3D11Texture2D& operator=(FDirect3D11Texture2D&&) & noexcept = default;
	virtual ~FDirect3D11Texture2D() noexcept { Texture.Reset(); }

public:
	inline bool IsValid() const noexcept { return Texture != nullptr; }

	inline DXGI_FORMAT Format() const noexcept { return Desc.Format; }
	template<typename T = std::size_t> inline T Width() const noexcept { return static_cast<T>(Desc.Width); }
	template<typename T = std::size_t> inline T Height() const noexcept { return static_cast<T>(Desc.Height); }

	inline ID3D11Texture2D* GetPtr() const noexcept { return Texture.Get(); }
	inline ID3D11Texture2D* const* GetAddressOf() const noexcept { return Texture.GetAddressOf(); }

private:
	TComPtr<ID3D11Texture2D> Texture;
	D3D11_TEXTURE2D_DESC Desc;
};

#endif
