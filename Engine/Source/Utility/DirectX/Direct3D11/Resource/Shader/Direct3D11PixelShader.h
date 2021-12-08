// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Shader/IPixelShader.h"
#include "Direct3D11ShaderReflection.h"

class FDirect3D11Renderer;

class FDirect3D11PixelShader final
	: public IPixelShader
	, private FDirect3D11ShaderReflection
{
public:
	FDirect3D11PixelShader(FDirect3D11Renderer& Renderer, ID3DBlob& CompiledPixelShaderObject);
	virtual ~FDirect3D11PixelShader() noexcept { Terminate(); }

private:
	virtual bool InitializeImpl(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept override final;
	inline virtual void TerminateImpl() noexcept override final { PixelShader.Reset(); }

	inline virtual bool IsValidImpl() const noexcept override final { return PixelShader != nullptr; }
	inline virtual void BindResourceImpl() const noexcept override final;

	inline virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final { Context.PSSetShader(PixelShader.Get(), nullptr, 0); }

public:
	inline virtual bool IsValid() const noexcept override final { return FDirect3D11ShaderReflection::IsValid(); }

	inline virtual std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept override final { return FDirect3D11ShaderReflection::QueryConstantBufferIndex(Name); }
	inline virtual FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept override final { return FDirect3D11ShaderReflection::QueryConstantBuffer(SlotIndex); }

private:
	FDirect3D11Renderer* Renderer;
	TComPtr<ID3D11PixelShader> PixelShader;
};

#endif
