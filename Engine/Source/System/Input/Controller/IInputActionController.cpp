// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IInputActionController.h"
#include "System/Input/InputContext.h"

bool IInputActionController::DispatchInputAction(
	const FInputContext& Context,
	FTimePoint Time,
	const FInputTrigger& Trigger) const
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
