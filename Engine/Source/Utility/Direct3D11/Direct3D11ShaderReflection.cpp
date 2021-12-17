// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11ShaderReflection.h"

#ifdef PLATFORM_WINDOWS

#include "Direct3D11Buffer.h"
#include "IDirect3D11ShaderResourceView.h"

std::size_t Direct3D11::ToByteSize(const D3D11_SIGNATURE_PARAMETER_DESC& SigParamDesc) noexcept
{
	if (false) {}
	else if (SigParamDesc.Mask & 0b1000) { return 16; }
	else if (SigParamDesc.Mask & 0b100) { return 12; }
	else if (SigParamDesc.Mask & 0b10) { return 8; }
	else if (SigParamDesc.Mask & 0b1) { return 4; }
	else { return 0; }
}

DXGI_FORMAT Direct3D11::ToDxgiFormat(const D3D11_SIGNATURE_PARAMETER_DESC& SigParamDesc) noexcept
{
	static constexpr auto TypeCount{ std::size_t{ 3 } };
	static constexpr auto SizeCount{ std::size_t{ 4 } };
	using Types = std::array<DXGI_FORMAT, TypeCount>;
	static constexpr std::array<Types, SizeCount> Formats
	{
		Types {
			DXGI_FORMAT::DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT::DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT },
		Types {
			DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT },
		Types {
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT },
		Types {
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT },
	};

	const auto ByteSize{ Direct3D11::ToByteSize(SigParamDesc) };
	if (ByteSize == 0
		|| ByteSize % 4 != 0
		|| SigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_UNKNOWN)
	{
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	const auto Size{ ByteSize / 4 - 1 };
	const auto Type{ static_cast<std::size_t>(SigParamDesc.ComponentType) - 1 };
	return Formats[Size][Type];
}

FDirect3D11ShaderReflection::FDirect3D11ShaderReflection(ID3D11Device& Device, ID3DBlob& ByteCode)
	: Reflector{}
	, ShaderDesc{}
	, CBufs{}
{
	if (!CreateReflection(Device, ByteCode))
	{
		DestroyReflection();
	}
}

FDirect3D11ShaderReflection::~FDirect3D11ShaderReflection() noexcept
{
	DestroyReflection();
}

FDirect3D11Buffer::Mapping FDirect3D11ShaderReflection::QueryConstantBuffer(std::size_t Index) const noexcept
{
	if (CBufs.Instances.at(Index) != nullptr)
	{
		return *CBufs.Instances.at(Index);
	}
	return {};
}

std::size_t FDirect3D11ShaderReflection::GetTexture2DSRVIndex(const std::string& Name) const noexcept
{
	if (SRVs.Names.contains(Name))
	{
		if (const auto [ViewDimension, Index] = SRVs.Names.at(Name);
			ViewDimension == FDirect3D11SRV_Texture2D::ViewDimension)
		{
			return Index;
		}
	}
	return Common::InvalidIndexValue;
}

FDirect3D11SRV_Texture2D::Mapping FDirect3D11ShaderReflection::QueryTexture2DSRV(std::size_t Index) const noexcept
{
	if (SRVs.Instances.Texture2Ds.contains(Index))
	{
		return *SRVs.Instances.Texture2Ds.at(Index);
	}
	return {};
}

void FDirect3D11ShaderReflection::BindResourceSlots(ID3D11DeviceContext& Context) const noexcept
{
	BindConstantBuffers(Context, CBufs.Slots.data(), static_cast<UINT>(CBufs.Slots.size()));
	BindShaderResourceViews(Context, SRVs.Slots.data(), static_cast<UINT>(SRVs.Slots.size()));
	BindSamplerStates(Context, reinterpret_cast<ID3D11SamplerState* const*>(Samplers.Slots.data()), static_cast<UINT>(Samplers.Slots.size()));
}

bool FDirect3D11ShaderReflection::CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(::D3DReflect(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		::IID_ID3D11ShaderReflection,
		&Reflector)))
	{
		return false;
	}

	if (FAILED(Reflector->GetDesc(&ShaderDesc)))
	{
		return false;
	}

	for (UINT ResIndex{}; ResIndex != ShaderDesc.BoundResources; ++ResIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC ResDesc{};
		if (FAILED(Reflector->GetResourceBindingDesc(ResIndex, &ResDesc)))
		{
			return false;
		}

		switch (ResDesc.Type)
		{
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			{
				if (ResDesc.BindPoint >= CBufs.Slots.size()
					|| CBufs.Slots[ResDesc.BindPoint] != nullptr
					|| CBufs.Names.contains(ResDesc.Name))
				{
					return false;
				}

				D3D11_SHADER_BUFFER_DESC ShaderBufDesc{};
				if (FAILED(Reflector->GetConstantBufferByIndex(ResDesc.BindPoint)->GetDesc(&ShaderBufDesc))
					|| ShaderBufDesc.Name == nullptr)
				{
					return false;
				}

				auto ConstantBuffer{ std::make_unique<FDirect3D11Buffer>(
					Device,
					D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
					D3D11_USAGE::D3D11_USAGE_DYNAMIC,
					D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
					1,
					ShaderBufDesc.Size,
					0,
					nullptr,
					0,
					0) };
				if (ConstantBuffer == nullptr || !ConstantBuffer->IsValid())
				{
					return false;
				}

				if (!CBufs.Names.emplace(ResDesc.Name, ResDesc.BindPoint).second)
				{
					return false;
				}

				CBufs.Slots[ResDesc.BindPoint] = ConstantBuffer->GetPtr();
				CBufs.Instances[ResDesc.BindPoint] = std::move(ConstantBuffer);
			}
			break;

			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER:
			{
				// @TODO: ShaderReflection_TextureBuffer
			}
			break;

			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			{
				if (ResDesc.Dimension == D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_UNKNOWN
					|| ResDesc.BindPoint >= SRVs.Slots.size()
					|| SRVs.Instances.Texture2Ds.contains(ResDesc.BindPoint)
					|| SRVs.Names.contains(ResDesc.Name))
				{
					return false;
				}

				if (!SRVs.Names.emplace(
					ResDesc.Name,
					std::make_pair(
						ResDesc.Dimension,
						ResDesc.BindPoint)).second)
				{
					return false;
				}

				switch (ResDesc.Dimension)
				{
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D:
					{
						auto Mapping{ std::make_unique<FDirect3D11SRV_Texture2D>(&SRVs.Slots[ResDesc.BindPoint]) };
						if (Mapping == nullptr || !Mapping->IsValid())
						{
							return false;
						}

						auto [It, bEmplaced] = SRVs.Instances.Texture2Ds.emplace(ResDesc.BindPoint, std::move(Mapping));
						if (!bEmplaced)
						{
							return false;
						}

						SRVs.Slots[ResDesc.BindPoint] = It->second->GetPtr();
					}
					break;

					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_BUFFER:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE1D:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE1DARRAY:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2DARRAY:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2DMS:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE3D:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURECUBE:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_BUFFEREX:
						break; // @TODO: ShaderReflection_Texture_SRVDimensions

					default:
					case D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_UNKNOWN:
						return false;
				}
			}
			break;

			case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			{
				D3D11_SAMPLER_DESC SamplerDesc{
					.Filter{ D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR },
					.AddressU{ D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP },
					.AddressV{ D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP },
					.AddressW{ D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP },
					.MipLODBias{},
					.MaxAnisotropy{},
					.ComparisonFunc{ D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS },
					.BorderColor{ 0.0f, 0.0f, 0.0f, 0.0f },
					.MinLOD{},
					.MaxLOD{} };
				if (FAILED(Device.CreateSamplerState(&SamplerDesc, &Samplers.Slots[ResDesc.BindPoint])))
				{
					Samplers.Slots[ResDesc.BindPoint].Reset();
					return false;
				}
			}
			break;

			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_RTACCELERATIONSTRUCTURE:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_FEEDBACKTEXTURE:
			{
				// @TODO: ShaderReflection_StructuredBuffer/UAV
			}
			break;

			default:
				return false;
		}
	}

	return true;
}

void FDirect3D11ShaderReflection::DestroyReflection() noexcept
{
	CBufs.Names.clear();
	CBufs.Instances = {};
	CBufs.Slots = {};

	SRVs.Names.clear();
	SRVs.Instances.Texture2Ds.clear();
	SRVs.Slots = {};

	Samplers.Slots = {};

	ShaderDesc = {};
	Reflector.Reset();
}

#endif
