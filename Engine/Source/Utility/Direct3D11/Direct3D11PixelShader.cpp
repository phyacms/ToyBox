// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11PixelShader.h"

#ifdef PLATFORM_WINDOWS

FDirect3D11PixelShader::FDirect3D11PixelShader(
	ID3D11Device& Device,
	ID3DBlob& ByteCode)
	: IDirect3D11Resource()
	, FDirect3D11ShaderReflection(Device, ByteCode)
	, PixelShader{}
{
	if (IsReflected())
	{
		if (!Iniitialize(Device, ByteCode))
		{
			Terminate();
		}
	}
}

bool FDirect3D11PixelShader::Iniitialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	return SUCCEEDED(Device.CreatePixelShader(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		nullptr,
		&PixelShader));
}

void FDirect3D11PixelShader::BindResourceImpl(ID3D11DeviceContext& Context) const noexcept
{
	Context.PSSetShader(PixelShader.Get(), nullptr, 0);
	BindResourceSlots(Context);
}

#endif
