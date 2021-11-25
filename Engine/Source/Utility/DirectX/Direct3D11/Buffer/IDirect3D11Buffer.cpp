// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11Buffer.h"

#ifdef PLATFORM_WINDOWS

IDirect3D11Buffer::IDirect3D11Buffer(
	ID3D11Device& Device,
	UINT BindFlags,
	D3D11_USAGE Usage,
	UINT CPUAccessFlags,
	const void* InitialSrcData,
	UINT ElementCount,
	UINT ElementSize)
{
	D3D11_BUFFER_DESC Desc
	{
		.ByteWidth{ ElementCount * ElementSize },
		.Usage{ Usage },
		.BindFlags{ BindFlags },
		.CPUAccessFlags{ CPUAccessFlags },
		.MiscFlags{},
		.StructureByteStride{ ElementSize }
	};
	D3D11_SUBRESOURCE_DATA SubRes
	{
		.pSysMem{ &InitialSrcData },
		.SysMemPitch{},
		.SysMemSlicePitch{}
	};
	if (FAILED(Device.CreateBuffer(
		&Desc,
		InitialSrcData != nullptr
			? &SubRes
			: nullptr,
		&Buffer)))
	{
		Buffer.Reset();
	}
}

#endif
