﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11VertexShader.h"

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11Renderer.h"
#include "Utility/DirectX/Direct3D11/Resource/Buffer/Direct3D11VertexShaderConstantBuffer.h"

FDirect3D11VertexShader::FDirect3D11VertexShader(
	FDirect3D11Renderer& Renderer,
	ID3DBlob& CompiledVertexShaderObject)
	: IVertexShader()
	, FDirect3D11ShaderReflection()
	, Renderer{ &Renderer }
	, VertexDesc{}
	, VertexShader{}
	, InputLayout{}
{
	if (!Initialize<FDirect3D11VertexShaderConstantBuffer>(Renderer, CompiledVertexShaderObject))
	{
		Terminate();
	}
}

bool FDirect3D11VertexShader::InitializeImpl(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept
{
	auto& Device{ Renderer.GetDevice() };
	return CreateVertexShader(Device, ByteCode)
		&& CreateInputLayout(Device, ByteCode);
}

void FDirect3D11VertexShader::TerminateImpl() noexcept
{
	VertexShader.Reset();
	InputLayout.Reset();
}

bool FDirect3D11VertexShader::IsValidImpl() const noexcept
{
	return !(false
		|| !VertexShader
		|| !InputLayout);
}

void FDirect3D11VertexShader::BindResourceImpl() const noexcept
{
	FDirect3D11ShaderReflection::BindResource(Renderer->GetDeviceContext());
}

void FDirect3D11VertexShader::BindResourceImpl(ID3D11DeviceContext& Context) const noexcept
{
	Context.IASetInputLayout(InputLayout.Get());
	Context.VSSetShader(VertexShader.Get(), nullptr, 0);
}

bool FDirect3D11VertexShader::CreateVertexShader(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(Device.CreateVertexShader(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		nullptr,
		&VertexShader)))
	{
		return false;
	}

	return true;
}

bool FDirect3D11VertexShader::CreateInputLayout(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	auto& Reflector{ GetShaderReflector() };
	const auto& ShaderDesc{ GetShaderDescriptions() };

	const auto InputParamCount{ ShaderDesc.InputParameters };
	if (InputParamCount == 0)
	{
		return false;
	}

	std::vector<D3D11_SIGNATURE_PARAMETER_DESC> ParamDescs{};
	ParamDescs.reserve(InputParamCount);
	for (UINT ParamIndex{}; ParamIndex != InputParamCount; ++ParamIndex)
	{
		D3D11_SIGNATURE_PARAMETER_DESC ParamDesc{};
		if (FAILED(Reflector.GetInputParameterDesc(ParamIndex, &ParamDesc)))
		{
			return false;
		}

		if (!std::string{ ParamDesc.SemanticName }.starts_with("SV_"))
		{
			ParamDescs.emplace_back(std::move(ParamDesc));
		}
	}

	static constexpr auto ParseInputElementSize{ [](BYTE Mask)->std::size_t
	{
		if (false) {}
		else if (Mask & 0b1000) { return 4; }
		else if (Mask & 0b100) { return 3; }
		else if (Mask & 0b10) { return 2; }
		else if (Mask & 0b1) { return 1; }
		else { return 0; }
	} };
	static constexpr auto ParseInputElementFormat{
		[](BYTE Mask, D3D_REGISTER_COMPONENT_TYPE ComponentType)->DXGI_FORMAT
		{
			const auto Size{ ParseInputElementSize(Mask) };
			if (Size == 0 || ComponentType == D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_UNKNOWN)
			{
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}

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

			const auto SizeIndex{ Size - 1 };
			const auto TypeIndex{ static_cast<std::size_t>(ComponentType) - 1 };
			return Formats[SizeIndex][TypeIndex];
		} };

	std::vector<D3D11_INPUT_ELEMENT_DESC> ElementDescs(ParamDescs.size());
	std::transform(
		std::execution::par_unseq,
		std::cbegin(ParamDescs),
		std::cend(ParamDescs),
		std::begin(ElementDescs),
		[](const D3D11_SIGNATURE_PARAMETER_DESC& SigParam)->D3D11_INPUT_ELEMENT_DESC
		{
			return D3D11_INPUT_ELEMENT_DESC
			{
				.SemanticName{ SigParam.SemanticName },
				.SemanticIndex{ SigParam.SemanticIndex },
				.Format{ ParseInputElementFormat(SigParam.Mask, SigParam.ComponentType) },
				.InputSlot{},
				.AlignedByteOffset{ D3D11_APPEND_ALIGNED_ELEMENT },
				.InputSlotClass{ D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA },
				.InstanceDataStepRate{}
			};
		}
	);

	if (FAILED(Device.CreateInputLayout(
		ElementDescs.data(),
		static_cast<UINT>(ElementDescs.size()),
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		&InputLayout)))
	{
		return false;
	}

	std::size_t OffsetPos{};
	for (std::size_t Index{}; Index != ElementDescs.size(); ++Index)
	{
		OffsetPos = VertexDesc.AddElementDesc(
			ElementDescs[Index].SemanticName,
			ElementDescs[Index].SemanticIndex,
			{ .ByteSize{ ParseInputElementSize(ParamDescs[Index].Mask) * 4 }, .OffsetPos{ OffsetPos } });
	}

	return true;
}

#endif
