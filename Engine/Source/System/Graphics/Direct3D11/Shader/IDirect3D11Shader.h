// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Type/Delegate.h"
#include "System/Graphics/Direct3D11/Direct3D11.h"

class IDirect3D11ShaderConstantBuffer;

class IDirect3D11Shader
{
public:
	IDirect3D11Shader();
	virtual ~IDirect3D11Shader() noexcept;

	IDirect3D11Shader(const IDirect3D11Shader&) = delete;
	IDirect3D11Shader& operator=(const IDirect3D11Shader&) & = delete;
	IDirect3D11Shader(IDirect3D11Shader&&) noexcept = delete;
	IDirect3D11Shader& operator=(IDirect3D11Shader&&) & noexcept = delete;

protected:
	bool Initialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	void Terminate() noexcept;

	inline bool IsReflected() const noexcept { return Reflector != nullptr; }

private:
	bool CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;

public:
	inline bool IsValid() const noexcept { return IsReflected() && IsValidImpl(); }
	void BindResource(ID3D11DeviceContext& Context) const noexcept;

	inline ID3D11ShaderReflection& GetShaderReflector() const noexcept { return *Reflector.Get(); }
	inline const D3D11_SHADER_DESC& GetShaderDescriptions() const noexcept { return ShaderDesc; }

private:
	virtual bool InitializeImpl(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept = 0;
	virtual void TerminateImpl() noexcept = 0;

	virtual bool IsValidImpl() const noexcept = 0;
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept = 0;

private:
	TComPtr<ID3D11ShaderReflection> Reflector;
	D3D11_SHADER_DESC ShaderDesc;
};

#endif
