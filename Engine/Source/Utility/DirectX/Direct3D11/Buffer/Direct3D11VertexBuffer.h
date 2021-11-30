// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11Buffer.h"
#include "System/Graphics/Data/DynamicVertex.h"

class FDirect3D11VertexBuffer
	: public IDirect3D11Buffer
{
public:
	FDirect3D11VertexBuffer(
		ID3D11Device& Device,
		const FVertexData& VertexData)
		: IDirect3D11Buffer(
			Device,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			VertexData.GetPtr(),
			static_cast<UINT>(VertexData.GetCount()),
			static_cast<UINT>(VertexData.GetStride()))
		, Stride(static_cast<UINT>(VertexData.GetStride())) {}
	virtual ~FDirect3D11VertexBuffer() noexcept = default;

private:
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final
	{
		UINT Offset{};
		Context.IASetVertexBuffers(0, 1, GetAddressOf(), &Stride, &Offset);
	}

private:
	const UINT Stride;
};

#endif
