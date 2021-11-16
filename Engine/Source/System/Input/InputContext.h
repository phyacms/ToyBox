// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/ScreenSpace.h"
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

	AInputController BindInputController(AObject<IInputController>&& Controller);
	void UnbindInputController(AInputController& Handle) noexcept;

private:
	FInput* Input;
	AObject<FSystemWindow> InputWindow;
	FDelegateHandles KeyboardMouseEvents;

	FKeyboardKeyStates KeyboardKeyStates;
	FMouseButtonStates MouseButtonStates;
	FScreenLocation MouseCursorLocation;
	std::queue<FInputEvent> InputEvents;

	FUniqueIdIssuer Issuer;
	std::list<std::pair<std::size_t, AObject<IInputController>>> Controllers;
};
