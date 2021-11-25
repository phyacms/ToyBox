// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11Shader.h"

class FDirect3D11PixelShader final
	: public IDirect3D11Shader
{
public:
	FDirect3D11PixelShader(ID3D11Device& Device, ID3DBlob& CompiledPixelShaderObject);
	virtual ~FDirect3D11PixelShader() noexcept { Terminate(); }

private:
	virtual bool InitializeImpl(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept override final;
	inline virtual void TerminateImpl() noexcept override final { PixelShader.Reset(); }

	inline virtual bool IsValidImpl() const noexcept override final { return PixelShader != nullptr; }
	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final { Context.PSSetShader(PixelShader.Get(), nullptr, 0); }

private:
	TComPtr<ID3D11PixelShader> PixelShader;
};

#endif
