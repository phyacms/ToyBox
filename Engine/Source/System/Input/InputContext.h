// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/ScreenSpace.h"
#include "System/Window/SystemWindow.h"
#include "KeyboardKey.h"
#include "MouseButton.h"

class FInput;

namespace InputEventTypes
{
	using FKeyboardKeyEvent = std::pair<EKeyboardKey, ESwitchEvent>;
	using FMouseButtonEvent = std::pair<EMouseButton, ESwitchEvent>;
	struct FMouseWheelUp final {};
	struct FMouseWheelDown final {};
	struct FMouseMovement final { FScreenLocation PrevCursorLocation{}; FScreenLocation CurrCursorLocation{}; };

	using FInputEvent = std::variant<
		FKeyboardKeyEvent,
		FMouseButtonEvent,
		FMouseWheelUp,
		FMouseWheelDown,
		FMouseMovement>;
}

class FInputContext final
{
public:
	FInputContext(
		FInput& Input,
		AObject<FSystemWindow>&& InputWindow);
	virtual ~FInputContext() noexcept;

	FInputContext(FInputContext&&) noexcept = delete;
	FInputContext& operator=(FInputContext&&) & noexcept = delete;
	FInputContext(const FInputContext&) = delete;
	FInputContext& operator=(const FInputContext&) & = delete;

public:
	bool IsValid() const noexcept;
	void ProcessInput();

	ESwitchState GetKeyboardKeyState(EKeyboardKey Key) const noexcept;
	ESwitchState GetMouseButtonState(EMouseButton Button) const noexcept;
	inline const FScreenLocation& GetMouseCursorLocation() const noexcept { return MouseCursorLocation; }

private:
	FInput* Input;
	AObject<FSystemWindow> InputWindow;
	FDelegateHandles KeyboardMouseEvents;

	FKeyboardKeyStates KeyboardKeyStates;
	FMouseButtonStates MouseButtonStates;
	FScreenLocation MouseCursorLocation;
	std::queue<InputEventTypes::FInputEvent> InputEvents;
};
