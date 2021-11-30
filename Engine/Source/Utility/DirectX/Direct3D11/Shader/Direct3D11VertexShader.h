// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "IDirect3D11Shader.h"
#include "System/Graphics/Data/DynamicVertex.h"

class FDirect3D11VertexShader final
	: public IDirect3D11Shader
{
public:
	FDirect3D11VertexShader(ID3D11Device& Device, ID3DBlob& CompiledVertexShaderObject);
	virtual ~FDirect3D11VertexShader() noexcept { Terminate(); }

private:
	virtual bool InitializeImpl(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept override final;
	virtual void TerminateImpl() noexcept override final;

	virtual bool IsValidImpl() const noexcept override final;
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override final;

	bool CreateVertexShader(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	bool CreateInputLayout(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;

public:
	inline const FVertexDesc& GetVertexDescription() const noexcept { return VertexDesc; }

private:
	FVertexDesc VertexDesc;
	TComPtr<ID3D11VertexShader> VertexShader;
	TComPtr<ID3D11InputLayout> InputLayout;
};

#endif
