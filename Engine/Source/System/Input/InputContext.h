﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/ScreenSpace.h"
#include "System/Window/SystemWindow.h"
#include "SwitchState.h"
#include "KeyboardKey.h"
#include "MouseButton.h"
#include "InputCode.h"
#include "PulseInput.h"

class FInput;

class FInputContext final
{
private:
	struct FPulseInput final
	{
		EPulseInput Event{};
		FInputCode TriggeredBy{};
		inline operator bool() const noexcept { return IsValid(); }
		bool IsValid() const noexcept;
	};
	struct FMouseMovement final { FScreenLocation CursorLocation{}; };

	using FInputEvent = std::variant<
		FPulseInput,
		FMouseMovement>;

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
	std::queue<FInputEvent> InputEvents;
};
