// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Buffer/IVertexBuffer.h"
#include "Direct3D11Buffer.h"

class FDirect3D11Renderer;

class FDirect3D11VertexBuffer
	: public IVertexBuffer
	, private FDirect3D11Buffer
{
public:
	FDirect3D11VertexBuffer(
		FDirect3D11Renderer& Renderer,
		const FVertexData& VertexData)
		: IVertexBuffer(VertexData)
		, FDirect3D11Buffer(
			Renderer,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			VertexData.GetPtr(),
			static_cast<UINT>(VertexData.GetCount()),
			static_cast<UINT>(VertexData.GetStride())) {}
	virtual ~FDirect3D11VertexBuffer() noexcept = default;

private:
	inline virtual bool IsValidImpl() const noexcept override final { return FDirect3D11Buffer::IsValid(); }
	inline virtual void BindResourceImpl() const noexcept override final { FDirect3D11Buffer::BindResource(); }

	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final
	{
		const auto Stride{ static_cast<UINT>(GetStride()) };
		const UINT Offset{};
		Context.IASetVertexBuffers(0, 1, GetAddressOf(), &Stride, &Offset);
	}
};

#endif
