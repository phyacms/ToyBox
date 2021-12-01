// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Buffer/IIndexBuffer.h"
#include "Direct3D11Buffer.h"

using FIndexData = std::vector<std::uint32_t>;

class FDirect3D11IndexBuffer
	: public IIndexBuffer
	, private FDirect3D11Buffer
{
public:
	FDirect3D11IndexBuffer(
		FDirect3D11Renderer& Renderer,
		const FIndexData& IndexData)
		: IIndexBuffer(IndexData)
		, FDirect3D11Buffer(
			Renderer,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			IndexData.data(),
			static_cast<UINT>(IndexData.size()),
			sizeof(FIndexData::value_type)) {}
	virtual ~FDirect3D11IndexBuffer() noexcept = default;

public:
	inline virtual bool BindResource() const noexcept override final { return FDirect3D11Buffer::BindResource(); }

private:
	inline virtual bool IsValidImpl() const noexcept override final { return FDirect3D11Buffer::IsValid(); }
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final
	{
		UINT Offset{};
		Context.IASetIndexBuffer(&GetBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	}
};

#endif
