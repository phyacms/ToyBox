// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Buffer/IDynamicBuffer.h"
#include "Direct3D11Buffer.h"

class IDirect3D11ShaderConstantBuffer
	: public IDynamicBuffer
	, private FDirect3D11Buffer
{
public:
	IDirect3D11ShaderConstantBuffer(
		FDirect3D11Renderer& Renderer,
		UINT DesiredSlot,
		UINT ByteWidth,
		const void* InitialSrcData = nullptr);
	virtual ~IDirect3D11ShaderConstantBuffer() noexcept = default;

public:
	inline virtual bool UpdateBuffer(const void* SrcData) override final { return FDirect3D11Buffer::UpdateBuffer(SrcData); }
	inline virtual bool BindResource() const noexcept override final { return FDirect3D11Buffer::BindResource(); }

	inline virtual std::size_t GetCount() const noexcept override final { return 1; }
	inline virtual std::size_t GetStride() const noexcept override final { return ByteWidth; }

	inline UINT GetDesiredSlot() const noexcept { return DesiredSlot; }

protected:
	using FDirect3D11Buffer::GetBuffer;
	using FDirect3D11Buffer::GetAddressOf;

private:
	inline virtual bool IsValidImpl() const noexcept override final { return FDirect3D11Buffer::IsValid(); }
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept override = 0;
	virtual bool UpdateBufferImpl(ID3D11DeviceContext& Context, const void* SrcData) override final;

private:
	UINT DesiredSlot;
	std::size_t ByteWidth;
};

#endif
