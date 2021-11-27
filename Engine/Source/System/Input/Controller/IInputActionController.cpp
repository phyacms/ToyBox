// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IInputActionController.h"
#include "System/Input/InputContext.h"

bool IInputActionController::DispatchKeyboardKeyEvent(
	const FInputContext& Context,
	EKeyboardKey Key,
	ESwitchEvent Event) const
{
	return DispatchInputAction(
		Context,
		FInputCodeTrigger{
			.InputCode{ Key },
			.Event{ Event } });
}

bool IInputActionController::DispatchMouseButtonEvent(
	const FInputContext& Context,
	EMouseButton Button,
	ESwitchEvent Event) const
{
	return DispatchInputAction(
		Context,
		FInputCodeTrigger{
			.InputCode{ Button },
			.Event{ Event } });
}

bool IInputActionController::DispatchMouseWheelMoveEvent(
	const FInputContext& Context,
	EMouseWheel Wheel) const
{
	return DispatchInputAction(Context, Wheel);
}

bool IInputActionController::DispatchInputAction(const FInputContext& Context, const FInputTrigger& Trigger) const
{
	const auto& cItBegin{ std::cbegin(Actions.second) };
	const auto& cItEnd{ std::cend(Actions.second) };

	const auto& Action{ std::find_if(
		std::execution::par_unseq,
		Actions.second.cbegin(),
		Actions.second.cend(),
		[&Context, &Trigger](const FInputAction& Action)->bool
		{
			if (Action.Chord.GetTrigger() == Trigger)
			{
				const auto& Modifiers{ Action.Chord.GetModifiers().InputCodes };
				return std::transform_reduce(
					std::execution::par_unseq,
					std::cbegin(Modifiers),
					std::cend(Modifiers),
					true,
					std::logical_and{},
					[&Context](const FInputCode& Modifier)->bool {
						return std::visit(stdhelp::overloaded{
							[&Context](EKeyboardKey Key)->bool { return Context.IsKeyboardKeyDown(Key); },
							[&Context](EMouseButton Button)->bool { return Context.IsMouseButtonDown(Button); }, },
							Modifier); });
			}
			return false;
		}) };
	if (Action != cItEnd
		&& Action->Callback.Execute(Context))
	{
		return true;
	}

	return false;
}
