// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11Buffer.h"

using FIndexData = std::vector<std::uint32_t>;

class FDirect3D11IndexBuffer
	: public IDirect3D11Buffer
{
public:
	FDirect3D11IndexBuffer(
		ID3D11Device& Device,
		const FIndexData& IndexData)
		: IDirect3D11Buffer(
			Device,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			{},
			IndexData.data(),
			static_cast<UINT>(IndexData.size()),
			sizeof(FIndexData::value_type))
		, IndexCount(static_cast<UINT>(IndexData.size())) {}
	virtual ~FDirect3D11IndexBuffer() noexcept = default;

public:
	inline UINT GetIndexCount() const noexcept { return IndexCount; }

private:
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final
	{
		UINT Offset{};
		Context.IASetIndexBuffer(&GetBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	}

private:
	UINT IndexCount;
};

#endif
