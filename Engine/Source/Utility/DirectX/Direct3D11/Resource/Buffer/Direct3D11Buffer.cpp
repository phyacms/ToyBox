// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11Buffer.h"

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11Renderer.h"

FDirect3D11Buffer::FDirect3D11Buffer(
	FDirect3D11Renderer& Renderer,
	UINT BindFlags,
	D3D11_USAGE Usage,
	UINT CPUAccessFlags,
	const void* InitialSrcData,
	UINT ElementCount,
	UINT ElementSize)
	: Renderer{ &Renderer }
	, Buffer{}
	, bDynamic{ (CPUAccessFlags & D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE) != 0 }
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
		.pSysMem{ InitialSrcData },
		.SysMemPitch{},
		.SysMemSlicePitch{}
	};
	if (FAILED(Renderer.GetDevice().CreateBuffer(
		&Desc,
		InitialSrcData != nullptr
			? &SubRes
			: nullptr,
		&Buffer)))
	{
		Buffer.Reset();
	}
}

void FDirect3D11Buffer::BindResource() const noexcept
{
	BindResourceImpl(Renderer->GetDeviceContext());
}

bool FDirect3D11Buffer::UpdateBuffer(const void* SrcData)
{
	return (IsValid() && IsDynamic())
		? UpdateBufferImpl(Renderer->GetDeviceContext(), SrcData)
		: false;
}

#endif
