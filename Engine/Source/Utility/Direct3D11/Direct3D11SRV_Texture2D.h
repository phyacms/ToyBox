// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Engine.h"
#include "Type/Common.h"
#include "Direct3D11.h"
#include "IDirect3D11ShaderResourceView.h"

class FDirect3D11ShaderReflection;

class FDirect3D11SRV_Texture2D final
	: public IDirect3D11ShaderResourceView
{
public:
	using ResourceType = ID3D11Texture2D;
	static constexpr auto ViewDimension{ D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D };

	class Proxy final
	{
	public:
		Proxy() : SRV{} {}
		Proxy(FDirect3D11SRV_Texture2D& SRV) : SRV{ &SRV } {}

	public:
		inline bool IsValid() const noexcept { return SRV != nullptr; }
		inline bool MapResource(ID3D11Device& Device, ResourceType& Resource) noexcept { return IsValid() ? SRV->MapResource(Device, Resource) : false; }

	private:
		FDirect3D11SRV_Texture2D* SRV;
	};

public:
	FDirect3D11SRV_Texture2D() : IDirect3D11ShaderResourceView() {}
	FDirect3D11SRV_Texture2D(ID3D11ShaderResourceView** Slot) : IDirect3D11ShaderResourceView(Slot) {}
	virtual ~FDirect3D11SRV_Texture2D() noexcept = default;

public:
	inline bool MapResource(ID3D11Device& Device, ResourceType& Resource) noexcept
	{
		D3D11_TEXTURE2D_DESC ResDesc{};
		Resource.GetDesc(&ResDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{
			.Format{ ResDesc.Format },
			.ViewDimension{ ViewDimension },
			.Texture2D{
				.MostDetailedMip{},
				.MipLevels{ 1 } } };

		if (!CreateShaderResourceView(Device, Resource, SRVDesc))
		{
			return false;
		}

		return true;
	}
};

#endif
