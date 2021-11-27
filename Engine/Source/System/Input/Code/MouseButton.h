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

namespace InputFunctions
{
	constexpr std::size_t MouseButtonCount{ static_cast<std::size_t>(EMouseButton::ENUM_END) };

	inline bool IsValidButton(EMouseButton Button) noexcept { return Button > EMouseButton::Invalid && Button < EMouseButton::ENUM_END; }
	inline std::size_t ToIndex(EMouseButton Button) noexcept { return IsValidButton(Button) ? static_cast<std::size_t>(Button) : MouseButtonCount; }
}

using FMouseButtonStates = std::array<ESwitchState, InputFunctions::MouseButtonCount>;
