// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/SystemWindow.h"
#include "Controller/IInputController.h"

class FInput;
class IInputActionController;

class FInputContext final
	: public TObject<FInputContext>
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

	ESwitchState GetKeyboardKeyState(EKeyboardKey Key) const noexcept;
	ESwitchState GetMouseButtonState(EMouseButton Button) const noexcept;
	inline const FScreenLocation& GetMouseCursorLocation() const noexcept { return MouseCursorLocation; }

	inline bool IsKeyboardKeyUp(EKeyboardKey Key) const noexcept { return GetKeyboardKeyState(Key) == ESwitchState::Up; }
	inline bool IsKeyboardKeyDown(EKeyboardKey Key) const noexcept { return GetKeyboardKeyState(Key) == ESwitchState::Down; }
	inline bool IsMouseButtonUp(EMouseButton Button) const noexcept { return GetMouseButtonState(Button) == ESwitchState::Up; }
	inline bool IsMouseButtonDown(EMouseButton Button) const noexcept { return GetMouseButtonState(Button) == ESwitchState::Down; }

	[[nodiscard]] AInputControllerBinding BindInputController(AObject<IInputController>&& Controller);
	[[nodiscard]] AInputControllerBinding BindInputController(IInputActionController& ActionController);
	void UnbindInputController(AInputControllerBinding& Handle) noexcept;

private:
	bool SetKeyboardKeyState(EKeyboardKey Key, ESwitchState State);
	bool SetMouseButtonState(EMouseButton Button, ESwitchState State);
	bool ConsumeMouseWheelMove(const FMouseWheelDelta& WheelDelta);
	void SetMouseCursorLocation(const FScreenLocation& CursorLocation);

private:
	FInput* Input;
	AObject<FSystemWindow> InputWindow;
	FDelegateHandles KeyboardMouseEvents;

	FKeyboardKeyStates KeyboardKeyStates;
	FMouseButtonStates MouseButtonStates;
	FScreenLocation MouseCursorLocation;

	FUniqueIdIssuer ControllerIdIssuer;
	std::list<std::pair<std::size_t, AObject<IInputController>>> Controllers;
};
