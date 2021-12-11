// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

class IPrimitiveTopology;

enum class EPrimitiveTopology
{
	Undefined,
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	AdjacentLineList,
	AdjacentLineStrip,
	AdjacentTriangleList,
	AdjacentTriangleStrip
};

using FPrimitiveTopologies = std::unordered_map<EPrimitiveTopology, std::unique_ptr<IPrimitiveTopology>>;
