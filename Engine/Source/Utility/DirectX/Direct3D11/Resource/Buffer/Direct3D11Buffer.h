// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11.h"

class FDirect3D11Renderer;

class FDirect3D11Buffer
{
public:
	FDirect3D11Buffer(
		FDirect3D11Renderer& Renderer,
		UINT BindFlags,
		D3D11_USAGE Usage,
		UINT CPUAccessFlags,
		const void* InitialSrcData,
		UINT ElementCount,
		UINT ElementSize);
	virtual ~FDirect3D11Buffer() noexcept { Buffer.Reset(); }

	FDirect3D11Buffer(const FDirect3D11Buffer&) = delete;
	FDirect3D11Buffer& operator=(const FDirect3D11Buffer&) & = delete;
	FDirect3D11Buffer(FDirect3D11Buffer&&) noexcept = delete;
	FDirect3D11Buffer& operator=(FDirect3D11Buffer&&) & noexcept = delete;

public:
	inline bool IsValid() const noexcept { return Buffer != nullptr; }
	void BindResource() const noexcept;

protected:
	inline ID3D11Buffer& GetBuffer() const noexcept { return *Buffer.Get(); }
	inline ID3D11Buffer* const* GetAddressOf() const noexcept { return Buffer.GetAddressOf(); }

	inline bool IsDynamic() const noexcept { return bDynamic; }
	bool UpdateBuffer(const void* SrcData);

private:
	virtual bool UpdateBufferImpl(ID3D11DeviceContext& Context, const void* SrcData) { return false; }
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept = 0;

private:
	FDirect3D11Renderer* Renderer;
	TComPtr<ID3D11Buffer> Buffer;
	bool bDynamic;
};

#endif
