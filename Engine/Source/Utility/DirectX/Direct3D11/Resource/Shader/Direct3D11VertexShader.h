// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Data/DynamicVertex.h"
#include "System/Graphics/Resource/Shader/IVertexShader.h"
#include "Direct3D11ShaderReflection.h"

class FDirect3D11Renderer;

class FDirect3D11VertexShader final
	: public IVertexShader
	, private FDirect3D11ShaderReflection
{
public:
	FDirect3D11VertexShader(FDirect3D11Renderer& Renderer, ID3DBlob& CompiledVertexShaderObject);
	virtual ~FDirect3D11VertexShader() noexcept { Terminate(); }

private:
	virtual bool InitializeImpl(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept override final;
	virtual void TerminateImpl() noexcept override final;

	virtual bool IsValidImpl() const noexcept override final;
	virtual void BindResourceImpl() const noexcept override final;

	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final;

	bool CreateVertexShader(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	bool CreateInputLayout(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;

public:
	inline virtual bool IsValid() const noexcept override final { return FDirect3D11ShaderReflection::IsValid(); }

	inline virtual std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept override final { return FDirect3D11ShaderReflection::QueryConstantBufferIndex(Name); }
	inline virtual FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept override final { return FDirect3D11ShaderReflection::QueryConstantBuffer(SlotIndex); }

	inline virtual const FVertexDesc& GetVertexDescription() const noexcept override final { return VertexDesc; }

private:
	FDirect3D11Renderer* Renderer;
	FVertexDesc VertexDesc;
	TComPtr<ID3D11VertexShader> VertexShader;
	TComPtr<ID3D11InputLayout> InputLayout;
};

#endif
