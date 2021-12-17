// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "IDirect3D11Resource.h"
#include "Direct3D11ShaderReflection.h"

class FDirect3D11PixelShader
	: public IDirect3D11Resource
	, private FDirect3D11ShaderReflection
{
public:
	FDirect3D11PixelShader(ID3D11Device& Device, ID3DBlob& ByteCode);
	virtual ~FDirect3D11PixelShader() noexcept { Terminate(); }

private:
	bool Initialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	inline void Terminate() noexcept { PixelShader.Reset(); }

public:
	inline virtual bool IsValid() const noexcept override final { return PixelShader != nullptr; }
	inline virtual void BindResource(ID3D11DeviceContext& Context) const noexcept override final { if (IsValid()) { BindResourceImpl(Context); } }

	using FDirect3D11ShaderReflection::GetConstantBufferIndex;
	using FDirect3D11ShaderReflection::QueryConstantBuffer;

	using FDirect3D11ShaderReflection::GetTexture2DSRVIndex;
	using FDirect3D11ShaderReflection::QueryTexture2DSRV;

private:
	inline virtual void BindConstantBuffers(ID3D11DeviceContext& Context, ID3D11Buffer* const* CBufs, UINT Count) const noexcept override final { Context.PSSetConstantBuffers(0, Count, CBufs); }
	inline virtual void BindShaderResourceViews(ID3D11DeviceContext& Context, ID3D11ShaderResourceView* const* SRVs, UINT Count) const noexcept override final { Context.PSSetShaderResources(0, Count, SRVs); }
	inline virtual void BindSamplerStates(ID3D11DeviceContext& Context, ID3D11SamplerState* const* Samplers, UINT Count) const noexcept override final { Context.PSSetSamplers(0, Count, Samplers); }

	void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept;

private:
	TComPtr<ID3D11PixelShader> PixelShader;
};

#endif
