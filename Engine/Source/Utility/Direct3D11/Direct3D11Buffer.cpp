// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11Buffer.h"

#ifdef PLATFORM_WINDOWS

#include "Direct3D11MappedSubResource.h"

FDirect3D11Buffer::FDirect3D11Buffer(
	ID3D11Device& Device,
	UINT BindFlags,
	D3D11_USAGE Usage,
	UINT CPUAccessFlags,
	UINT StructureCount,
	UINT StructureByteStride,
	UINT MiscFlags,
	const void* InitialSrcData,
	UINT RowPitch,
	UINT SlicePitch)
	: Buffer{}
	, bDynamic{ (Usage & D3D11_USAGE::D3D11_USAGE_DYNAMIC) != 0 }
	, bWritable{ (CPUAccessFlags & D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE) != 0 }
	, Stride{ StructureByteStride }
	, ByteSize{ Stride * StructureCount }
{
	D3D11_BUFFER_DESC Desc{
		.ByteWidth{ GetByteSize<UINT>() },
		.Usage{ Usage },
		.BindFlags{ BindFlags },
		.CPUAccessFlags{ CPUAccessFlags },
		.MiscFlags{ MiscFlags },
		.StructureByteStride{ GetStride<UINT>() } };

	D3D11_SUBRESOURCE_DATA SubRes{
		.pSysMem{ InitialSrcData },
		.SysMemPitch{ RowPitch },
		.SysMemSlicePitch{ SlicePitch } };

	if (FAILED(Device.CreateBuffer(
		&Desc,
		InitialSrcData != nullptr ? &SubRes : nullptr,
		&Buffer)))
	{
		Buffer.Reset();
	}
}

bool FDirect3D11Buffer::UpdateBuffer(
	ID3D11DeviceContext& Context,
	const void* SrcData,
	std::size_t ByteSize) noexcept
{
	if (!IsDynamic() || !IsWritable())
	{
		return false;
	}

	if (const auto BufferSize{ GetByteSize() };
		ByteSize > BufferSize)
	{
		ByteSize = BufferSize;
	}

	return FDirect3D11MappedSubResource{
		Context,
		*GetPtr(),
		0,
		D3D11_MAP::D3D11_MAP_WRITE_DISCARD,
		0 }.Map(SrcData, ByteSize);
}

#endif
