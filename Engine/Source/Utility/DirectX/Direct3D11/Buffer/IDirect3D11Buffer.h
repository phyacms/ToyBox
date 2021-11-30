// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11.h"

class IDirect3D11Buffer
{
public:
	IDirect3D11Buffer(
		ID3D11Device& Device,
		UINT BindFlags,
		D3D11_USAGE Usage,
		UINT CPUAccessFlags,
		const void* InitialSrcData,
		UINT ElementCount,
		UINT ElementSize);
	virtual ~IDirect3D11Buffer() noexcept { Buffer.Reset(); }

	IDirect3D11Buffer(const IDirect3D11Buffer&) = delete;
	IDirect3D11Buffer& operator=(const IDirect3D11Buffer&) & = delete;
	IDirect3D11Buffer(IDirect3D11Buffer&&) noexcept = delete;
	IDirect3D11Buffer& operator=(IDirect3D11Buffer&&) & noexcept = delete;

public:
	inline bool IsValid() const noexcept { return Buffer != nullptr; }
	inline void BindResource(ID3D11DeviceContext& Context) const noexcept { if (IsValid()) { BindResourceImpl(Context); } }

protected:
	inline ID3D11Buffer& GetBuffer() const noexcept { return *Buffer.Get(); }
	inline ID3D11Buffer* const* GetAddressOf() const noexcept { return Buffer.GetAddressOf(); }

private:
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept = 0;

private:
	TComPtr<ID3D11Buffer> Buffer;
};

#endif
