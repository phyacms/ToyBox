// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SwitchState.h"

enum class EPulseInput : std::size_t
{
	Idle     = static_cast<std::size_t>(ESwitchEvent::Idle),
	Pressed  = static_cast<std::size_t>(ESwitchEvent::Pressed),
	Released = static_cast<std::size_t>(ESwitchEvent::Released),
	Repeated = static_cast<std::size_t>(ESwitchEvent::Repeated),
	RolledDown,
	RolledUp,
};
