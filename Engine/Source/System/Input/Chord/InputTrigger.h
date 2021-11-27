// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "System/Input/Code/SwitchState.h"
#include "System/Input/Code/InputCode.h"
#include "System/Input/Code/MouseWheel.h"

struct FInputCodeTrigger
{
	FInputCode InputCode{};
	ESwitchEvent Event{};
	friend inline bool operator!=(const FInputCodeTrigger& Lhs, const FInputCodeTrigger& Rhs) noexcept = default;
	friend inline bool operator==(const FInputCodeTrigger& Lhs, const FInputCodeTrigger& Rhs) noexcept = default;
	inline operator bool() const noexcept { return IsValid(); }
	inline bool IsValid() const noexcept { return InputFunctions::IsValidInputCode(InputCode) && Event != ESwitchEvent::Idle; }
};

using FInputTrigger = std::variant<FInputCodeTrigger, EMouseWheel>;

namespace InputFunctions
{
	bool IsValidTrigger(const FInputTrigger& Trigger) noexcept;
}
