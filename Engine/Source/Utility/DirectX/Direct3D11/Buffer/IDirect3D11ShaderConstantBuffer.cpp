// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11ShaderConstantBuffer.h"

#ifdef PLATFORM_WINDOWS

IDirect3D11ShaderConstantBuffer::IDirect3D11ShaderConstantBuffer(
	ID3D11Device& Device,
	UINT DesiredSlot,
	UINT ByteWidth,
	const void* InitialSrcData)
	: IDirect3D11Buffer(
		Device,
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
		InitialSrcData,
		1,
		ByteWidth)
	, DesiredSlot{ DesiredSlot }
	, ByteWidth{ ByteWidth }
{
}

IDirect3D11ShaderConstantBuffer& IDirect3D11ShaderConstantBuffer::Update(ID3D11DeviceContext& Context, const void* SrcData)
{
	struct FMappedSubResource final
	{
		ID3D11DeviceContext* Context;
		ID3D11Buffer* Resource;
		D3D11_MAPPED_SUBRESOURCE MappedSubRes;
		bool bMapped;
		FMappedSubResource(ID3D11DeviceContext& Context, ID3D11Buffer& Resource)
			: Context{ &Context }
			, Resource{ &Resource }
			, MappedSubRes{}
			, bMapped{ SUCCEEDED(Context.Map(&Resource, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubRes)) } {}
		~FMappedSubResource() noexcept { if (bMapped) { Context->Unmap(Resource, 0); } }
		void Map(const void* SrcData, std::size_t ByteWidth) { if (bMapped) { std::memcpy(MappedSubRes.pData, SrcData, ByteWidth); } }
	};
	FMappedSubResource(Context, GetBuffer()).Map(SrcData, ByteWidth);
	return *this;
}

#endif
