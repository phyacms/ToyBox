// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "KeyboardKey.h"
#include "MouseButton.h"

using FInputCode = std::variant<EKeyboardKey, EMouseButton>;

namespace InputFunctions
{
	bool IsValidInputCode(const FInputCode& InputCode) noexcept;
}

struct FInputCodes final
{
	std::unordered_set<FInputCode> InputCodes{};
	friend bool operator==(const FInputCodes&, const FInputCodes&) = default;
	friend bool operator!=(const FInputCodes&, const FInputCodes&) = default;
	inline FInputCodes& operator+=(const FInputCode& InputCode) { InputCodes.emplace(InputCode); return *this; }
	inline FInputCodes& operator+=(FInputCode&& InputCode) { InputCodes.emplace(std::move(InputCode)); return *this; }
	inline FInputCodes& operator-=(const FInputCode& InputCode) noexcept { InputCodes.erase(InputCode); return *this; }
	inline FInputCodes& operator-=(FInputCode&& InputCode) noexcept { InputCodes.erase(std::move(InputCode)); return *this; }
};
