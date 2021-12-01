// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11ShaderReflection.h"

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Resource/Buffer/IDirect3D11ShaderConstantBuffer.h"

FDirect3D11ShaderReflection::FDirect3D11ShaderReflection()
	: Reflector{}
	, ShaderDesc{}
	, ConstBuf{}
{
}

FDirect3D11ShaderReflection::~FDirect3D11ShaderReflection() noexcept
{
}

void FDirect3D11ShaderReflection::Terminate() noexcept
{
	ConstBuf.BufDescs.clear();
	ConstBuf.InputBindDescs.clear();
	ConstBuf.Objects.clear();
	TerminateImpl();
	Reflector.Reset();
}

bool FDirect3D11ShaderReflection::CreateReflection(ID3DBlob& ByteCode) noexcept
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

bool FDirect3D11ShaderReflection::ReflectConstantBuffer() noexcept
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

bool FDirect3D11ShaderReflection::AddConstantBuffer(std::unique_ptr<IDirect3D11ShaderConstantBuffer>&& Created) noexcept
{
	auto SlotIndex{ Created->GetDesiredSlot() };
	const auto& [cIt, bEmplaced] = ConstBuf.Objects.emplace(SlotIndex, std::move(Created));
	return bEmplaced;
}

bool FDirect3D11ShaderReflection::BindResource(ID3D11DeviceContext& Context) const noexcept
{
	if (IsValid())
	{
		BindResourceImpl(Context);
		std::for_each(
			std::execution::seq,
			std::cbegin(ConstBuf.Objects),
			std::cend(ConstBuf.Objects),
			[&Context](const auto& Pair)->void { Pair.second->BindResource(); });
		return true;
	}
	return false;
}

std::size_t FDirect3D11ShaderReflection::QueryConstantBufferIndex(std::string_view Name) const noexcept
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

FDynamicBuffer FDirect3D11ShaderReflection::QueryConstantBuffer(std::size_t SlotIndex) const& noexcept
{
	return ConstBuf.Objects.contains(SlotIndex)
		? *ConstBuf.Objects.at(SlotIndex)
		: FDynamicBuffer{};
}

#endif
