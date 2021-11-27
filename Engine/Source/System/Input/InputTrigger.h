// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "SwitchState.h"
#include "InputCode.h"

struct FInputCodeTrigger
{
	FInputCode InputCode{};
	ESwitchEvent Event{};
	friend inline bool operator!=(const FInputCodeTrigger& Lhs, const FInputCodeTrigger& Rhs) noexcept = default;
	friend inline bool operator==(const FInputCodeTrigger& Lhs, const FInputCodeTrigger& Rhs) noexcept = default;
	inline operator bool() const noexcept { return IsValid(); }
	inline bool IsValid() const noexcept { return InputFunctions::IsValidInputCode(InputCode) && Event != ESwitchEvent::Idle; }
};

enum class EMouseWheelTrigger : std::size_t
{
	RollUp,
	RollDown,
};

using FMouseWheelTriggers = std::vector<EMouseWheelTrigger>;

using FInputTrigger = std::variant<FInputCodeTrigger, EMouseWheelTrigger>;

namespace InputFunctions
{
	bool IsValidTrigger(const FInputTrigger& Trigger) noexcept;
}
