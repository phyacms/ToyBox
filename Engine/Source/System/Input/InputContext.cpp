// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputContext.h"
#include "IInputController.h"

FInputContext::FInputContext(
	FInput& Input,
	AObject<FSystemWindow>&& InputWindow)
	: TObject<FInputContext>(*this)
	, Input{ &Input }
	, InputWindow{ std::move(InputWindow) }
	, KeyboardMouseEvents{}
	, KeyboardKeyStates{}
	, MouseButtonStates{}
	, MouseCursorLocation{}
	, Issuer{}
	, Controllers{}
{
	if (this->InputWindow.IsValid())
	{
		using namespace SystemWindowEventTypes;

		KeyboardMouseEvents += this->InputWindow->Events.OnKeyboardKey.AddDynamic(
			[this](const FOnKeyboardKey& EventArgs)->bool
			{
				if (InputFunctions::IsValidKey(EventArgs.Key))
				{
					const auto Index{ InputFunctions::ToIndex(EventArgs.Key) };
					const auto Event{ ::ToSwitchEvent(GetKeyboardKeyState(EventArgs.Key), EventArgs.State) };
					KeyboardKeyStates[Index] = EventArgs.State;
					if (Event != ESwitchEvent::Idle)
					{
						KeyboardKeyStates[Index] = EventArgs.State;
						return DispatchInputAction(FInputCodeTrigger{ .InputCode{ EventArgs.Key }, .Event{ Event } });
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseButton.AddDynamic(
			[this](const FOnMouseButton& EventArgs)->bool
			{
				if (InputFunctions::IsValidButton(EventArgs.Button))
				{
					const auto Index{ InputFunctions::ToIndex(EventArgs.Button) };
					const auto Event{ ::ToSwitchEvent(GetMouseButtonState(EventArgs.Button), EventArgs.State) };
					MouseButtonStates[Index] = EventArgs.State;
					if (Event != ESwitchEvent::Idle)
					{
						return DispatchInputAction(FInputCodeTrigger{
							.InputCode{ EventArgs.Button },
							.Event{ Event } });
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseWheel.AddDynamic(
			[this](const FOnMouseWheel& EventArgs)->bool
			{
				for (auto WheelMove : EventArgs.WheelDelta)
				{
					if (DispatchInputAction(WheelMove))
					{
						return true;
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseMove.AddDynamic(
			[this](const FOnMouseMove& EventArgs)->bool
			{
				MouseCursorLocation = EventArgs.CursorLocation;
				return false;
			});
	}
}

FInputContext::~FInputContext() noexcept
{
	InputWindow.Release();
}

bool FInputContext::IsValid() const noexcept
{
	return InputWindow.IsValid();
}

ESwitchState FInputContext::GetKeyboardKeyState(EKeyboardKey Key) const noexcept
{
	return InputFunctions::IsValidKey(Key)
		? KeyboardKeyStates.at(InputFunctions::ToIndex(Key))
		: ESwitchState::Up;
}

ESwitchState FInputContext::GetMouseButtonState(EMouseButton Button) const noexcept
{
	return InputFunctions::IsValidButton(Button)
		? MouseButtonStates.at(InputFunctions::ToIndex(Button))
		: ESwitchState::Up;
}

AInputController FInputContext::BindInputController(AObject<IInputController>&& Controller)
{
	if (Controller.IsValid())
	{
		Controller->BindStatics();

		AUniqueId Issued{ Issuer.Issue() };
		Controllers.emplace_front(
			std::make_pair(
				Issued.GetHash(),
				std::move(Controller)));
		return Issued;
	}

	return {};
}

void FInputContext::UnbindInputController(AInputController& Handle) noexcept
{
	if (auto Index{ Handle.GetHash() };
		Handle.IsValid() && Index != FUniqueId::InvalidId)
	{
		Controllers.erase(
			std::find_if(
				std::execution::par_unseq,
				std::cbegin(Controllers),
				std::cend(Controllers),
				[Index](const auto& Pair)->bool { return Pair.first == Index; }));
		Handle.Release();
	}
}

bool FInputContext::DispatchInputAction(const FInputTrigger& Trigger)
{
	for (const auto& [Id, Controller] : Controllers)
	{
		const auto& Action{ std::find_if(
			std::execution::par_unseq,
			Controller->cbegin(),
			Controller->cend(),
			[this, &Trigger](const FInputAction& Action)->bool
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
						[this](const FInputCode& Modifier)->bool
						{
							return std::visit(stdhelp::overloaded{
								[this](EKeyboardKey Key)->bool { return IsKeyboardKeyDown(Key); },
								[this](EMouseButton Button)->bool { return IsMouseButtonDown(Button); }, },
								Modifier); });
				}
				return false;
			}) };
		if (Action != Controller->cend()
			&& Action->Callback.Execute(*this))
		{
			return true;
		}
	}
	return false;
}
