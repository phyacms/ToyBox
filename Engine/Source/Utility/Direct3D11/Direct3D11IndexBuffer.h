// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Data/DynamicVertex.h"
#include "IDirect3D11Resource.h"
#include "Direct3D11Buffer.h"

class FDirect3D11IndexBuffer
	: public IDirect3D11Resource
{
public:
	FDirect3D11IndexBuffer(ID3D11Device& Device, const FIndexData& IndexData)
		: IDirect3D11Resource()
		, Count{ IndexData.size() }
		, Buffer{
			Device,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			GetCount<UINT>(),
			sizeof(FIndexData::value_type),
			{},
			IndexData.data(),
			0,
			0 } {}
	virtual ~FDirect3D11IndexBuffer() noexcept = default;

public:
	inline virtual bool IsValid() const noexcept override final { return Buffer.IsValid(); }
	inline virtual void BindResource(ID3D11DeviceContext& Context) const noexcept override final { if (IsValid()) { BindResourceImpl(Context); } }

	template<typename T = std::size_t> inline T GetCount() const noexcept { return static_cast<T>(Count); }

private:
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept
	{
		Context.IASetIndexBuffer(Buffer.GetPtr(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	}

private:
	std::size_t Count;
	FDirect3D11Buffer Buffer;
};

#endif
