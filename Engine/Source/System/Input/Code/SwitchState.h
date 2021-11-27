// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

enum class ESwitchState : std::size_t { Up, Down };
enum class ESwitchEvent : std::size_t { Idle, Pressed, Released, Repeated };

inline ESwitchEvent ToSwitchEvent(ESwitchState Prev, ESwitchState Curr) noexcept
{
	switch (
		((0b1 & static_cast<std::size_t>(Prev)) << 1)
		| (0b1 & static_cast<std::size_t>(Curr)))
	{
		default:
		case 0b00: return ESwitchEvent::Idle;
		case 0b01: return ESwitchEvent::Pressed;
		case 0b10: return ESwitchEvent::Released;
		case 0b11: return ESwitchEvent::Repeated;
	}
}
