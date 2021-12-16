// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"

class FDirect3D11MappedSubResource final
{
public:
	FDirect3D11MappedSubResource(
		ID3D11DeviceContext& Context,
		ID3D11Resource& Resource,
		UINT SubResource,
		D3D11_MAP MapType,
		UINT MapFlags)
		: Context{ &Context }
		, Resource{ &Resource }
		, MappedSubRes{}
		, bMapped{ SUCCEEDED(Context.Map(
			&Resource,
			SubResource,
			MapType,
			MapFlags,
			&MappedSubRes)) } {}
	~FDirect3D11MappedSubResource() noexcept { if (bMapped) { Context->Unmap(Resource, 0); } }

	FDirect3D11MappedSubResource(const FDirect3D11MappedSubResource&) = delete;
	FDirect3D11MappedSubResource& operator=(const FDirect3D11MappedSubResource&) & = delete;
	FDirect3D11MappedSubResource(FDirect3D11MappedSubResource&&) noexcept = delete;
	FDirect3D11MappedSubResource& operator=(FDirect3D11MappedSubResource&&) & noexcept = delete;

public:
	inline bool Map(const void* SrcData, std::size_t ByteSize)
	{
		if (bMapped)
		{
			std::memcpy(MappedSubRes.pData, SrcData, ByteSize);
			return true;
		}
		return false;
	}

private:
	ID3D11DeviceContext* Context;
	ID3D11Resource* Resource;
	D3D11_MAPPED_SUBRESOURCE MappedSubRes;
	bool bMapped;
};

#endif
