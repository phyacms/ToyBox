// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"

class IDirect3D11Resource
{
public:
	IDirect3D11Resource() = default;
	virtual ~IDirect3D11Resource() noexcept = default;

	IDirect3D11Resource(const IDirect3D11Resource&) = delete;
	IDirect3D11Resource& operator=(const IDirect3D11Resource&) & = delete;
	IDirect3D11Resource(IDirect3D11Resource&&) noexcept = delete;
	IDirect3D11Resource& operator=(IDirect3D11Resource&&) & noexcept = delete;

public:
	virtual bool IsValid() const noexcept = 0;
	virtual void BindResource(ID3D11DeviceContext& Context) const noexcept = 0;
};

#endif
