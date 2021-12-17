// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Engine.h"
#include "Type/Common.h"
#include "Direct3D11.h"
#include "IDirect3D11ShaderResourceView.h"

class FDirect3D11Texture2D;

class FDirect3D11SRV_Texture2D final
	: public IDirect3D11ShaderResourceView
{
public:
	static constexpr auto ViewDimension{ D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D };

	class Mapping final
	{
	public:
		Mapping() : SRV{} {}
		Mapping(FDirect3D11SRV_Texture2D& SRV) : SRV{ &SRV } {}

	public:
		inline bool IsValid() const noexcept { return SRV != nullptr; }
		inline bool MapResource(ID3D11Device& Device, const FDirect3D11Texture2D& Texture) noexcept { return IsValid() ? SRV->MapResource(Device, Texture) : false; }

	private:
		FDirect3D11SRV_Texture2D* SRV;
	};

public:
	FDirect3D11SRV_Texture2D() : IDirect3D11ShaderResourceView() {}
	FDirect3D11SRV_Texture2D(ID3D11ShaderResourceView** Slot) : IDirect3D11ShaderResourceView(Slot) {}
	virtual ~FDirect3D11SRV_Texture2D() noexcept = default;

public:
	bool MapResource(ID3D11Device& Device, const FDirect3D11Texture2D& Texture) noexcept;
};

#endif
