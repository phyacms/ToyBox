// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/Delegate/DelegateHandle.h"
#include "Type/ScreenSpace/ScreenLocation.h"
#include "Controller/IInputController.h"

class FInput;
class IInputActionController;
class FSystemWindow;
struct FOnKeyboardKey;
struct FOnMouseButton;
struct FOnMouseWheel;
struct FOnMouseMove;

class FInputContext final
	: public TObject<FInputContext>
{
public:
	FInputContext(
		FInput& Input,
		FSystemWindow& InputWindow);
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

	[[nodiscard]] AInputControllerBinding BindInputController(IInputController& Controller);
	[[nodiscard]] AInputControllerBinding BindInputController(IInputActionController& ActionController);
	void UnbindInputController(AInputControllerBinding& Handle) noexcept;

private:
	bool SetKeyboardKeyState(const FOnKeyboardKey& EventArgs);
	bool SetMouseButtonState(const FOnMouseButton& EventArgs);
	bool ConsumeMouseWheel(const FOnMouseWheel& EventArgs);
	bool ConsumeMouseMove(const FOnMouseMove& EventArgs);

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
