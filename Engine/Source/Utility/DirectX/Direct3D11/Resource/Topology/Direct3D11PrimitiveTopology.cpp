// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Direct3D11PrimitiveTopology.h"

#ifdef PLATFORM_WINDOWS

#include "Utility/DirectX/Direct3D11/Direct3D11Renderer.h"

void FDirect3D11PrimitiveTopology::BindResourceImpl() const noexcept
{
	static constexpr auto ToPrimitiveTopology{
		[](EPrimitiveTopology Topology)->D3D11_PRIMITIVE_TOPOLOGY
		{
			switch (Topology)
			{
				default:
				case EPrimitiveTopology::Undefined: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
				case EPrimitiveTopology::PointList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				case EPrimitiveTopology::LineList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				case EPrimitiveTopology::LineStrip: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
				case EPrimitiveTopology::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				case EPrimitiveTopology::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
				case EPrimitiveTopology::AdjacentLineList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
				case EPrimitiveTopology::AdjacentLineStrip: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
				case EPrimitiveTopology::AdjacentTriangleList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
				case EPrimitiveTopology::AdjacentTriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
			}
		} };

	Renderer->GetDeviceContext().IASetPrimitiveTopology(ToPrimitiveTopology(GetTopology()));
}

#endif
