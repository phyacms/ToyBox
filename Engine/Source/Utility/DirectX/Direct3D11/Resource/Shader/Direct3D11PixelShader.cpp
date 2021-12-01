// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11PixelShader.h"

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11Renderer.h"
#include "Utility/DirectX/Direct3D11/Resource/Buffer/Direct3D11PixelShaderConstantBuffer.h"

FDirect3D11PixelShader::FDirect3D11PixelShader(
	FDirect3D11Renderer& Renderer,
	ID3DBlob& CompiledPixelShaderObject)
	: IPixelShader()
	, FDirect3D11ShaderReflection()
	, Renderer{ &Renderer }
{
	if (!Initialize<FDirect3D11PixelShaderConstantBuffer>(Renderer, CompiledPixelShaderObject))
	{
		Terminate();
	}
}

bool FDirect3D11PixelShader::InitializeImpl(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(Renderer.GetDevice().CreatePixelShader(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		nullptr,
		&PixelShader)))
	{
		return false;
	}

	return true;
}

bool FDirect3D11PixelShader::BindResource() const noexcept
{
	return FDirect3D11ShaderReflection::BindResource(Renderer->GetDeviceContext());
}

#endif
