// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11Buffer.h"

class IDirect3D11ShaderConstantBuffer
	: public IDirect3D11Buffer
{
public:
	IDirect3D11ShaderConstantBuffer(
		ID3D11Device& Device,
		UINT DesiredSlot,
		UINT ByteWidth,
		const void* InitialSrcData = nullptr);
	virtual ~IDirect3D11ShaderConstantBuffer() noexcept = default;

public:
	inline UINT GetDesiredSlot() const noexcept { return DesiredSlot; }
	inline UINT GetByteWidth() const noexcept { return ByteWidth; }
	IDirect3D11ShaderConstantBuffer& Update(ID3D11DeviceContext& Context, const void* SrcData);

private:
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override = 0;

private:
	const UINT DesiredSlot;
	const UINT ByteWidth;
};

#endif
