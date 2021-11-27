// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "IInputController.h"
#include "System/Input/Chord/InputAction.h"

// @TODO: Modify to bind/unbind actions dynamically.
class IInputActionController
	: public IInputController
{
public:
	IInputActionController() : IInputController(), Actions{} {}
	virtual ~IInputActionController() noexcept = default;

	IInputActionController(const IInputActionController&) = delete;
	IInputActionController& operator=(const IInputActionController&) & = delete;
	IInputActionController(IInputActionController&&) noexcept = delete;
	IInputActionController& operator=(IInputActionController&&) & noexcept = delete;

public:
	inline void BindStatics() {
		std::call_once(
			Actions.first,
			[this]()->void { BindInputActions(Actions.second); }); }

private:
	virtual void BindInputActions(FInputActionBindings& Actions) = 0;

	virtual bool DispatchKeyboardKeyEvent(const FInputContext& Context, EKeyboardKey Key, ESwitchEvent Event) const override final;
	virtual bool DispatchMouseButtonEvent(const FInputContext& Context, EMouseButton Button, ESwitchEvent Event) const override final;
	virtual bool DispatchMouseWheelMoveEvent(const FInputContext& Context, EMouseWheel Wheel) const override final;

	bool DispatchInputAction(const FInputContext& Context, const FInputTrigger& Trigger) const;

private:
	std::pair<std::once_flag, FInputActionBindings> Actions;
};
