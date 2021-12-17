// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Data/DynamicVertex.h"
#include "Direct3D11.h"
#include "IDirect3D11Resource.h"
#include "Direct3D11ShaderReflection.h"

namespace HLSL::Semantics::VertexShader::Input
{
#define DeclareSemantic(Name_, bIndexed_, Type_)		\
	typedef struct Semantic_##Name_ final				\
	{													\
		inline static constexpr auto Name{ #Name_ };	\
		inline static bool bIndexed{ bIndexed_ };		\
		using Type = Type_;								\
	}													\
	##Name_;

	DeclareSemantic(BINORMAL, true, float[4])
	DeclareSemantic(BLENDINDICES, true, std::uint32_t)
	DeclareSemantic(BLENDWEIGHT, true, float)
	DeclareSemantic(COLOR, true, float[4])
	DeclareSemantic(NORMAL, true, float[4])
	DeclareSemantic(POSITION, true, float[4])
	DeclareSemantic(POSITIONT, false, float[4])
	DeclareSemantic(PSIZE, true, float)
	DeclareSemantic(TANGENT, true, float[4])
	DeclareSemantic(TEXCOORD, true, float[4])

#undef DeclareSemantic
}

class FDirect3D11VertexShader
	: public IDirect3D11Resource
	, private FDirect3D11ShaderReflection
{
public:
	FDirect3D11VertexShader(ID3D11Device& Device, ID3DBlob& ByteCode);
	virtual ~FDirect3D11VertexShader() noexcept { Terminate(); }

private:
	bool Initialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	inline void Terminate() noexcept { InputLayout.Reset(); VertexShader.Reset(); }

public:
	inline virtual bool IsValid() const noexcept override final { return InputLayout != nullptr && VertexShader != nullptr; }
	inline virtual void BindResource(ID3D11DeviceContext& Context) const noexcept override final { if (IsValid()) { BindResourceImpl(Context); } }

	using FDirect3D11ShaderReflection::GetConstantBufferIndex;
	using FDirect3D11ShaderReflection::QueryConstantBuffer;

	inline const FVertexDesc& GetVertexDescription() const noexcept { return VertexDesc; }

private:
	inline virtual void BindConstantBuffers(ID3D11DeviceContext& Context, ID3D11Buffer* const* CBufs, UINT Count) const noexcept override final { Context.VSSetConstantBuffers(0, Count, CBufs); }
	inline virtual void BindShaderResourceViews(ID3D11DeviceContext& Context, ID3D11ShaderResourceView* const* SRVs, UINT Count) const noexcept override final { Context.VSSetShaderResources(0, Count, SRVs); }
	inline virtual void BindSamplerStates(ID3D11DeviceContext& Context, ID3D11SamplerState* const* Samplers, UINT Count) const noexcept override final { Context.VSSetSamplers(0, Count, Samplers); }

	void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept;

private:
	FVertexDesc VertexDesc;
	TComPtr<ID3D11InputLayout> InputLayout;
	TComPtr<ID3D11VertexShader> VertexShader;
};

#endif
