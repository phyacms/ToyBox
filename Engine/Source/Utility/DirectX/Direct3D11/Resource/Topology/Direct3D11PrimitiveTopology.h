// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Topology/IPrimitiveTopology.h"

class FDirect3D11Renderer;

class FDirect3D11PrimitiveTopology final
	: public IPrimitiveTopology
{
public:
	FDirect3D11PrimitiveTopology(
		FDirect3D11Renderer& Renderer,
		EPrimitiveTopology Topology)
		: IPrimitiveTopology(Topology)
		, Renderer{ &Renderer } {}
	virtual ~FDirect3D11PrimitiveTopology() noexcept = default;

private:
	virtual void BindResourceImpl() const noexcept override final;

private:
	FDirect3D11Renderer* Renderer;
};

#endif
