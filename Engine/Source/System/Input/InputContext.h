// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/SystemWindow.h"
#include "IInputController.h"

class FInput;
class IInputController;
class FInputContext;

class FInputContext final
	: public TObject<FInputContext>
{
private:
	struct FMouseMovement final { FScreenLocation CursorLocation{}; };

	using FInputEvent = std::variant<
		FInputTrigger,
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

	ESwitchState GetKeyboardKeyState(EKeyboardKey Key) const noexcept;
	ESwitchState GetMouseButtonState(EMouseButton Button) const noexcept;
	inline const FScreenLocation& GetMouseCursorLocation() const noexcept { return MouseCursorLocation; }

	inline bool IsKeyboardKeyUp(EKeyboardKey Key) const noexcept { return GetKeyboardKeyState(Key) == ESwitchState::Up; }
	inline bool IsKeyboardKeyDown(EKeyboardKey Key) const noexcept { return GetKeyboardKeyState(Key) == ESwitchState::Down; }
	inline bool IsMouseButtonUp(EMouseButton Button) const noexcept { return GetMouseButtonState(Button) == ESwitchState::Up; }
	inline bool IsMouseButtonDown(EMouseButton Button) const noexcept { return GetMouseButtonState(Button) == ESwitchState::Down; }

	[[nodiscard]] AInputController BindInputController(AObject<IInputController>&& Controller);
	void UnbindInputController(AInputController& Handle) noexcept;

private:
	bool DispatchInputAction(const FInputTrigger& Trigger);

private:
	FInput* Input;
	AObject<FSystemWindow> InputWindow;
	FDelegateHandles KeyboardMouseEvents;

	FKeyboardKeyStates KeyboardKeyStates;
	FMouseButtonStates MouseButtonStates;
	FScreenLocation MouseCursorLocation;

	FUniqueIdIssuer Issuer;
	std::list<std::pair<std::size_t, AObject<IInputController>>> Controllers;
};
