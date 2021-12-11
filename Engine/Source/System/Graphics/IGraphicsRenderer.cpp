// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IGraphicsRenderer.h"
#include "Resource/Topology/IPrimitiveTopology.h"

bool IGraphicsRenderer::CreateStaticResources() noexcept
{
	auto& Topologies{ IPrimitiveTopology::GetStatic() };

	Topologies.Clear();
	for (auto Topology : Topologies.Targets)
	{
		Topologies.Instances[Topology] = CreatePrimitiveTopology(Topology);
	}
	if (!Topologies.IsValid())
	{
		return false;
	}

	return true;
}
