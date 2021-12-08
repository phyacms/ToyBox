// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

namespace Common
{
	struct InvalidIndex final { inline static constexpr auto Value{ static_cast<std::size_t>(-1) }; };
	inline constexpr auto InvalidIndexValue{ InvalidIndex::Value };
}
