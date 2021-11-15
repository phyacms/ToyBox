// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11Shader.h"

#ifdef PLATFORM_WINDOWS

IDirect3D11Shader::IDirect3D11Shader()
	: Reflector{}
	, ShaderDesc{}
{
}

IDirect3D11Shader::~IDirect3D11Shader() noexcept
{
}

bool IDirect3D11Shader::Initialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	return CreateReflection(Device, ByteCode)
		&& InitializeImpl(Device, ByteCode);
}

void IDirect3D11Shader::Terminate() noexcept
{
	TerminateImpl();
	Reflector.Reset();
}

bool IDirect3D11Shader::CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(::D3DReflect(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		IID_ID3D11ShaderReflection,
		&Reflector)))
	{
		return false;
	}

	if (FAILED(Reflector->GetDesc(&ShaderDesc)))
	{
		return false;
	}

	return true;
}

void IDirect3D11Shader::BindResource(ID3D11DeviceContext& Context) const noexcept
{
	if (IsValid())
	{
		BindResourceImpl(Context);
	}
}

#endif
