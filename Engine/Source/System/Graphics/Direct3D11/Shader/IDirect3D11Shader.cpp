// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11Shader.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Direct3D11/Buffer/IDirect3D11ShaderConstantBuffer.h"

IDirect3D11Shader::IDirect3D11Shader()
	: Reflector{}
	, ShaderDesc{}
	, ConstantBuffer{}
{
}

IDirect3D11Shader::~IDirect3D11Shader() noexcept
{
}

void IDirect3D11Shader::Terminate() noexcept
{
	ConstantBuffer.BufDescs.clear();
	ConstantBuffer.InputBindDescs.clear();
	ConstantBuffer.Objects.clear();
	TerminateImpl();
	Reflector.Reset();
}

bool IDirect3D11Shader::CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
{
	if (FAILED(::D3DReflect(
		ByteCode.GetBufferPointer(),
		ByteCode.GetBufferSize(),
		IID_ID3D11ShaderReflection,
		&Reflector)))
	{
		return false;
	}

	if (FAILED(Reflector->GetDesc(&ShaderDesc)))
	{
		return false;
	}

	return true;
}

bool IDirect3D11Shader::ReflectConstantBuffer() noexcept
{
	static constexpr auto CheckResults{
		[](const auto& hResults)->bool {
			return std::transform_reduce(
				std::execution::par_unseq,
				std::cbegin(hResults),
				std::cend(hResults),
				true,
				std::logical_and<bool>(),
				[](HRESULT hResult)->bool { return SUCCEEDED(hResult); }); } };
	std::vector<HRESULT> hResults(ShaderDesc.ConstantBuffers);

	ConstantBuffer.BufDescs.resize(hResults.size());
	{
		std::atomic_size_t BufIndex{};
		std::for_each(
			std::execution::par_unseq,
			std::begin(ConstantBuffer.BufDescs),
			std::end(ConstantBuffer.BufDescs),
			[this, &hResults, &BufIndex](D3D11_SHADER_BUFFER_DESC& BufDesc)->void
			{
				auto Index{ BufIndex.fetch_add(1, std::memory_order_relaxed) };
				hResults[Index] = Reflector->GetConstantBufferByIndex(static_cast<UINT>(Index))->GetDesc(&BufDesc);
			});
		if (!CheckResults(hResults))
		{
			return false;
		}
	}

	ConstantBuffer.InputBindDescs.resize(hResults.size());
	{
		std::atomic_size_t BufIndex{};
		std::for_each(
			std::execution::par_unseq,
			std::cbegin(ConstantBuffer.BufDescs),
			std::cend(ConstantBuffer.BufDescs),
			[this, &hResults, &BufIndex](const D3D11_SHADER_BUFFER_DESC& BufDesc)->void
			{
				auto Index{ BufIndex.fetch_add(1, std::memory_order_relaxed) };
				hResults[Index] = Reflector->GetResourceBindingDescByName(BufDesc.Name, &ConstantBuffer.InputBindDescs[Index]);
			});
		if (!CheckResults(hResults))
		{
			return false;
		}
	}

	return true;
}

bool IDirect3D11Shader::AddConstantBuffer(std::unique_ptr<IDirect3D11ShaderConstantBuffer>&& Created) noexcept
{
	auto SlotIndex{ Created->GetDesiredSlot() };
	const auto& [cIt, bEmplaced] = ConstantBuffer.Objects.emplace(SlotIndex, std::move(Created));
	return bEmplaced;
}

void IDirect3D11Shader::BindResource(ID3D11DeviceContext& Context) const noexcept
{
	if (IsValid())
	{
		BindResourceImpl(Context);
		std::for_each(
			std::execution::seq,
			std::cbegin(ConstantBuffer.Objects),
			std::cend(ConstantBuffer.Objects),
			[&Context](const auto& Pair)->void { Pair.second->BindResource(Context); });
	}
}

#endif
