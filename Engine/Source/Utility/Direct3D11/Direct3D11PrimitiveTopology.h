// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Engine.h"
#include "Direct3D11.h"
#include "IDirect3D11Resource.h"

class FDirect3D11Renderer;

class FDirect3D11PrimitiveTopology final
	: public IDirect3D11Resource
{
public:
	FDirect3D11PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) : IDirect3D11Resource(), Topology{ Topology } {}
	~FDirect3D11PrimitiveTopology() noexcept = default;

public:
	inline virtual bool IsValid() const noexcept override final { return true; }
	inline virtual void BindResource(ID3D11DeviceContext& Context) const noexcept override final { Context.IASetPrimitiveTopology(Topology); }

private:
	D3D11_PRIMITIVE_TOPOLOGY Topology;
};

#endif
