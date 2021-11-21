// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11Shader.h"

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Direct3D11/Buffer/IDirect3D11ShaderConstantBuffer.h"

IDirect3D11Shader::IDirect3D11Shader()
	: Reflector{}
	, ShaderDesc{}
	, ConstBuf{}
{
}

IDirect3D11Shader::~IDirect3D11Shader() noexcept
{
}

void IDirect3D11Shader::Terminate() noexcept
{
	ConstBuf.BufDescs.clear();
	ConstBuf.InputBindDescs.clear();
	ConstBuf.Objects.clear();
	TerminateImpl();
	Reflector.Reset();
}

bool IDirect3D11Shader::CreateReflection(ID3D11Device& Device, ID3DBlob& ByteCode) noexcept
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

	ConstBuf.BufDescs.resize(hResults.size());
	{
		std::atomic_size_t BufIndex{};
		std::for_each(
			std::execution::par_unseq,
			std::begin(ConstBuf.BufDescs),
			std::end(ConstBuf.BufDescs),
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

	ConstBuf.InputBindDescs.resize(hResults.size());
	{
		std::atomic_size_t BufIndex{};
		std::for_each(
			std::execution::par_unseq,
			std::cbegin(ConstBuf.BufDescs),
			std::cend(ConstBuf.BufDescs),
			[this, &hResults, &BufIndex](const D3D11_SHADER_BUFFER_DESC& BufDesc)->void
			{
				auto Index{ BufIndex.fetch_add(1, std::memory_order_relaxed) };
				hResults[Index] = Reflector->GetResourceBindingDescByName(BufDesc.Name, &ConstBuf.InputBindDescs[Index]);
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
	const auto& [cIt, bEmplaced] = ConstBuf.Objects.emplace(SlotIndex, std::move(Created));
	return bEmplaced;
}

void IDirect3D11Shader::BindResource(ID3D11DeviceContext& Context) const noexcept
{
	if (IsValid())
	{
		BindResourceImpl(Context);
		std::for_each(
			std::execution::seq,
			std::cbegin(ConstBuf.Objects),
			std::cend(ConstBuf.Objects),
			[&Context](const auto& Pair)->void { Pair.second->BindResource(Context); });
	}
}

IDirect3D11Shader::ConstantBufferRef IDirect3D11Shader::QueryConstantBuffer(std::size_t SlotIndex) const& noexcept
{
	if (ConstBuf.Objects.contains(SlotIndex)) { return std::ref(*ConstBuf.Objects.at(SlotIndex)); }
	else { return std::nullopt; }
}

std::size_t IDirect3D11Shader::QueryConstantBufferIndex(std::string_view Name) const noexcept
{
	auto cIt{ std::find_if(
		std::execution::par_unseq,
		std::cbegin(ConstBuf.BufDescs),
		std::cend(ConstBuf.BufDescs),
		[&Name](const D3D11_SHADER_BUFFER_DESC& BufDesc)->bool { return Name == BufDesc.Name; }) };
	return cIt != std::cend(ConstBuf.BufDescs)
		? cIt - std::cbegin(ConstBuf.BufDescs)
		: InvalidIndex;
}

#endif
