// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Type/Common.h"
#include "Direct3D11.h"

class FDirect3D11ShaderReflection;

class IDirect3D11ShaderResourceView
{
public:
	IDirect3D11ShaderResourceView(ID3D11ShaderResourceView** Slot = nullptr)
		: Slot{ Slot }
		, SRV{}
		, LastUsed{} {}
	virtual ~IDirect3D11ShaderResourceView() noexcept { Clear(); }

	IDirect3D11ShaderResourceView(const IDirect3D11ShaderResourceView&) = delete;
	IDirect3D11ShaderResourceView& operator=(const IDirect3D11ShaderResourceView&) & = delete;
	IDirect3D11ShaderResourceView(IDirect3D11ShaderResourceView&&) noexcept = delete;
	IDirect3D11ShaderResourceView& operator=(IDirect3D11ShaderResourceView&&) & noexcept = delete;

public:
	inline bool IsValid() const noexcept { return Slot != nullptr; }

	inline ID3D11ShaderResourceView* GetPtr() const noexcept { return SRV.Get(); }
	inline ID3D11ShaderResourceView** GetAddressOf() noexcept { return SRV.GetAddressOf(); }
	inline ID3D11ShaderResourceView* const* GetAddressOf() const noexcept { return SRV.GetAddressOf(); }

protected:
	bool CreateShaderResourceView(
		ID3D11Device& Device,
		ID3D11Resource& Resource,
		const D3D11_SHADER_RESOURCE_VIEW_DESC& Desc) noexcept;

private:
	inline void BindSlot() noexcept { *Slot = SRV.Get(); }

public:
	inline void Clear() noexcept { SRV.Reset(); LastUsed = {}; }

private:
	ID3D11ShaderResourceView** Slot;
	TComPtr<ID3D11ShaderResourceView> SRV;
	std::uintptr_t LastUsed;
};

#endif
