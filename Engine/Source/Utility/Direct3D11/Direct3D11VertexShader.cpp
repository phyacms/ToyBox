// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11VertexShader.h"

#ifdef PLATFORM_WINDOWS

FDirect3D11VertexShader::FDirect3D11VertexShader(
	ID3D11Device& Device,
	ID3DBlob& ByteCode)
	: IDirect3D11Resource()
	, FDirect3D11ShaderReflection(Device, ByteCode)
	, VertexShader{}
{
	if (IsReflected())
	{
		if (!Iniitialize(Device, ByteCode))
		{
			Terminate();
		}
	}
}

bool FDirect3D11VertexShader::Iniitialize(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(Device.CreateVertexShader(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		nullptr,
		&VertexShader)))
	{
		return false;
	}

	const auto InputParamCount{ GetDescription().InputParameters };
	if (InputParamCount == 0)
	{
		return false;
	}

	std::vector<D3D11_SIGNATURE_PARAMETER_DESC> SigParamDescs{};
	SigParamDescs.reserve(InputParamCount);

	auto& Reflector{ GetReflector() };
	for (UINT ParamIndex{}; ParamIndex != InputParamCount; ++ParamIndex)
	{
		D3D11_SIGNATURE_PARAMETER_DESC SigParamDesc{};
		if (FAILED(Reflector.GetInputParameterDesc(ParamIndex, &SigParamDesc)))
		{
			return false;
		}

		if (!std::string{ SigParamDesc.SemanticName }.starts_with("SV_"))
		{
			SigParamDescs.emplace_back(std::move(SigParamDesc));
		}
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> ElemDescs(SigParamDescs.size());
	std::transform(
		std::execution::par_unseq,
		std::cbegin(SigParamDescs),
		std::cend(SigParamDescs),
		std::begin(ElemDescs),
		[](const D3D11_SIGNATURE_PARAMETER_DESC& SigParamDesc)->D3D11_INPUT_ELEMENT_DESC
		{
			return D3D11_INPUT_ELEMENT_DESC
			{
				.SemanticName{ SigParamDesc.SemanticName },
				.SemanticIndex{ SigParamDesc.SemanticIndex },
				.Format{ Direct3D11::ToDxgiFormat(SigParamDesc) },
				.InputSlot{},
				.AlignedByteOffset{ D3D11_APPEND_ALIGNED_ELEMENT },
				.InputSlotClass{ D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA },
				.InstanceDataStepRate{}
			};
		}
	);

	if (FAILED(Device.CreateInputLayout(
		ElemDescs.data(),
		static_cast<UINT>(ElemDescs.size()),
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		&InputLayout)))
	{
		return false;
	}

	std::size_t OffsetPos{};
	for (std::size_t ElemIndex{}; ElemIndex != ElemDescs.size(); ++ElemIndex)
	{
		const auto ByteSize{ Direct3D11::ToByteSize(SigParamDescs[ElemIndex]) };
		if (ByteSize == 0 || ByteSize % 4 != 0)
		{
			return false;
		}

		OffsetPos = VertexDesc.AddElementDesc(
			ElemDescs[ElemIndex].SemanticName,
			ElemDescs[ElemIndex].SemanticIndex,
			{ .ByteSize{ ByteSize }, .OffsetPos{ OffsetPos } });
	}
	if (!VertexDesc.IsValid())
	{
		return false;
	}

	return true;
}

void FDirect3D11VertexShader::BindResourceImpl(ID3D11DeviceContext& Context) const noexcept
{
	Context.IASetInputLayout(InputLayout.Get());
	Context.VSSetShader(VertexShader.Get(), nullptr, 0);
	BindResourceSlots(Context);
}

#endif
