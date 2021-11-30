// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11ShaderConstantBuffer.h"

class FDirect3D11VertexShaderConstantBuffer final
	: public IDirect3D11ShaderConstantBuffer
{
public:
	FDirect3D11VertexShaderConstantBuffer(
		ID3D11Device& Device,
		UINT DesiredSlot,
		UINT ByteWidth,
		const void* InitialSrcData = nullptr)
		: IDirect3D11ShaderConstantBuffer(Device, DesiredSlot, ByteWidth, InitialSrcData) {}
	virtual ~FDirect3D11VertexShaderConstantBuffer() noexcept = default;

private:
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final
	{
		Context.VSSetConstantBuffers(GetDesiredSlot(), 1, GetAddressOf());
	}
};

#endif
