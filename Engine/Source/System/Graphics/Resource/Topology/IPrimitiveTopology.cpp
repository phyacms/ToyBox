// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IPrimitiveTopology.h"

const IPrimitiveTopology::FStatic& IPrimitiveTopology::GetStatic() noexcept
{
	static FStatic Topologies{};
	return Topologies;
}

bool IPrimitiveTopology::FStatic::IsValid() const noexcept
{
	return std::transform_reduce(
		std::execution::par_unseq,
		std::cbegin(Instances),
		std::cend(Instances),
		true,
		std::logical_and<bool>{},
		[](const auto& Pair)->bool { return Pair.second != nullptr && Pair.second->IsValid(); });
}
