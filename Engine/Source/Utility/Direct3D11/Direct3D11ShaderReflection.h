// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Type/Common.h"
#include "Direct3D11.h"
#include "Direct3D11Buffer.h"
#include "Direct3D11SRV_Texture2D.h"

namespace Direct3D11
{
	std::size_t ToByteSize(const D3D11_SIGNATURE_PARAMETER_DESC& SigParamDesc) noexcept;
	DXGI_FORMAT ToDxgiFormat(const D3D11_SIGNATURE_PARAMETER_DESC& SigParamDesc) noexcept;
}

class FDirect3D11ShaderReflection
{
public:
	FDirect3D11ShaderReflection(ID3D11Device& Device, ID3DBlob& ByteCode);
	FDirect3D11ShaderReflection(const FDirect3D11ShaderReflection&) = default;
	FDirect3D11ShaderReflection& operator=(const FDirect3D11ShaderReflection&) & = default;
	FDirect3D11ShaderReflection(FDirect3D11ShaderReflection&&) noexcept = default;
	FDirect3D11ShaderReflection& operator=(FDirect3D11ShaderReflection&&) & noexcept = default;
	virtual ~FDirect3D11ShaderReflection() noexcept;

public:
	inline bool IsReflected() const noexcept { return Reflector != nullptr; };
	inline ID3D11ShaderReflection& GetReflector() const& noexcept { return *Reflector.Get(); }
	inline const D3D11_SHADER_DESC& GetDescription() const noexcept { return ShaderDesc; }

protected:
	inline std::size_t GetConstantBufferIndex(const std::string& Name) const noexcept { return CBufs.Names.contains(Name) ? CBufs.Names.at(Name) : Common::InvalidIndexValue; }
	inline FDirect3D11Buffer::Writer QueryConstantBuffer(const std::string& Name) const noexcept { return QueryConstantBuffer(GetConstantBufferIndex(Name)); }
	FDirect3D11Buffer::Writer QueryConstantBuffer(std::size_t Index) const noexcept;

	std::size_t GetTexture2DSRVIndex(const std::string& Name) const noexcept;
	inline FDirect3D11SRV_Texture2D::Proxy QueryTexture2DSRV(const std::string& Name) const noexcept { return QueryTexture2DSRV(GetTexture2DSRVIndex(Name)); }
	FDirect3D11SRV_Texture2D::Proxy QueryTexture2DSRV(std::size_t Index) const noexcept;

	void BindResourceSlots(ID3D11DeviceContext& Context) const noexcept;

private:
	bool CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept;
	void DestroyReflection() noexcept;

	virtual void BindConstantBuffers(ID3D11DeviceContext& Context, ID3D11Buffer* const* CBufs, UINT Count) const noexcept = 0;
	virtual void BindShaderResourceViews(ID3D11DeviceContext& Context, ID3D11ShaderResourceView* const* SRVs, UINT Count) const noexcept = 0;
	virtual void BindSamplerStates(ID3D11DeviceContext& Context, ID3D11SamplerState* const* Samplers, UINT Count) const noexcept = 0;

private:
	TComPtr<ID3D11ShaderReflection> Reflector;
	D3D11_SHADER_DESC ShaderDesc;

	struct
	{
		inline static constexpr auto SlotCount{ D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT - 1 };
		std::unordered_map<std::string, std::size_t> Names{};
		std::array<std::unique_ptr<FDirect3D11Buffer>, SlotCount> Instances{};
		std::array<ID3D11Buffer*, SlotCount> Slots{};
	}
	CBufs;

	struct
	{
		inline static constexpr auto SlotCount{ D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT };
		std::unordered_map<std::string, std::pair<D3D_SRV_DIMENSION, std::size_t>> Names{};
		struct
		{
			std::unordered_map<std::size_t, std::unique_ptr<FDirect3D11SRV_Texture2D>> Texture2Ds{};
		}
		Instances{};
		std::array<ID3D11ShaderResourceView*, SlotCount> Slots{};
	}
	SRVs;

	struct
	{
		inline static constexpr auto SlotCount{ D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT };
		std::array<TComPtr<ID3D11SamplerState>, SlotCount> Slots{};
	}
	Samplers;
};

#endif
