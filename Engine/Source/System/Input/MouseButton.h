// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SwitchState.h"

enum class EMouseButton : std::size_t
{
	Invalid = std::size_t{},

	Left, Right, Middle,
	Front, Back,

	ENUM_END
};

namespace MouseFunctions
{
	constexpr std::size_t ButtonCount{ static_cast<std::size_t>(EMouseButton::ENUM_END) };

	inline bool IsValidButton(EMouseButton ButtonCode) noexcept { return ButtonCode > EMouseButton::Invalid && ButtonCode < EMouseButton::ENUM_END; }
	inline std::size_t ButtonCodeToIndex(EMouseButton ButtonCode) noexcept { return IsValidButton(ButtonCode) ? static_cast<std::size_t>(ButtonCode) : ButtonCount; }
}

using FMouseButtonStates = std::array<ESwitchState, MouseFunctions::ButtonCount>;
