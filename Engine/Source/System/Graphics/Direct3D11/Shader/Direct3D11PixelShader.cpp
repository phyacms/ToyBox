// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11PixelShader.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Direct3D11/Buffer/Direct3D11PixelShaderConstantBuffer.h"

FDirect3D11PixelShader::FDirect3D11PixelShader(ID3D11Device& Device, ID3DBlob& CompiledPixelShaderObject)
	: IDirect3D11Shader()
	, PixelShader{}
{
	if (!Initialize<FDirect3D11PixelShaderConstantBuffer>(Device, CompiledPixelShaderObject))
	{
		Terminate();
	}
}

bool FDirect3D11PixelShader::InitializeImpl(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(Device.CreatePixelShader(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		nullptr,
		&PixelShader)))
	{
		return false;
	}

	return true;
}

#endif
