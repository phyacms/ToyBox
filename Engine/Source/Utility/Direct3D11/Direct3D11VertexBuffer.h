// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Data/DynamicVertex.h"
#include "IDirect3D11Resource.h"
#include "Direct3D11Buffer.h"

class FDirect3D11VertexBuffer
	: public IDirect3D11Resource
{
public:
	FDirect3D11VertexBuffer(ID3D11Device& Device, const FVertexData& VertexData)
		: IDirect3D11Resource()
		, Count{ VertexData.GetCount() }
		, Buffer{
			Device,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			GetCount<UINT>(),
			static_cast<UINT>(VertexData.GetStride()),
			{},
			VertexData.GetPtr(),
			0,
			0 } {}
	virtual ~FDirect3D11VertexBuffer() noexcept = default;

public:
	inline virtual bool IsValid() const noexcept override final { return Buffer.IsValid(); }
	inline virtual void BindResource(ID3D11DeviceContext& Context) const noexcept override final { if (IsValid()) { BindResourceImpl(Context); } }

	template<typename T = std::size_t> inline T GetCount() const noexcept { return static_cast<T>(Count); }
	template<typename T = std::size_t> inline T GetStride() const noexcept { return Buffer.GetStride<T>(); }

private:
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept
	{
		const auto Stride{ GetStride<UINT>() };
		const UINT Offset{};
		Context.IASetVertexBuffers(0, 1, Buffer.GetAddressOf(), &Stride, &Offset);
	}

private:
	std::size_t Count;
	FDirect3D11Buffer Buffer;
};

#endif
