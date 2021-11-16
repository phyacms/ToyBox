// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputContext.h"

FInputContext::FInputContext(
	FInput& Input,
	AObject<FSystemWindow>&& InputWindow)
	: Input{ &Input }
	, InputWindow{ std::move(InputWindow) }
	, KeyboardMouseEvents{}
	, KeyboardKeyStates{}
	, MouseButtonStates{}
	, MouseCursorLocation{}
	, InputEvents{}
{
	if (this->InputWindow.IsValid())
	{
		using namespace SystemWindowEventTypes;

		KeyboardMouseEvents += this->InputWindow->Events.OnKeyboardKey.AddDynamic(
			[this](const FOnKeyboardKey& EventArgs)->bool
			{
				if (KeyboardFunctions::IsValidKey(EventArgs.Key))
				{
					auto& KeyState{ KeyboardKeyStates.at(static_cast<std::size_t>(EventArgs.Key)) };
					const auto Event{ ::ToSwitchEvent(KeyState, EventArgs.State) };
					if (Event != ESwitchEvent::Idle)
					{
						InputEvents.emplace(InputEventTypes::FKeyboardKeyEvent{ EventArgs.Key, Event });
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseButton.AddDynamic(
			[this](const FOnMouseButton& EventArgs)->bool
			{
				if (MouseFunctions::IsValidButton(EventArgs.Button))
				{
					auto& ButtonState{ MouseButtonStates.at(static_cast<std::size_t>(EventArgs.Button)) };
					const auto Event{ ::ToSwitchEvent(ButtonState, EventArgs.State) };
					if (Event != ESwitchEvent::Idle)
					{
						InputEvents.emplace(InputEventTypes::FMouseButtonEvent{ EventArgs.Button, Event });
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseWheel.AddDynamic(
			[this](const FOnMouseWheel& EventArgs)->bool
			{
				auto WheelDelta{ EventArgs.WheelDelta };
				while (WheelDelta-- > 0) { InputEvents.emplace(InputEventTypes::FMouseWheelUp{}); }
				while (WheelDelta++ < 0) { InputEvents.emplace(InputEventTypes::FMouseWheelDown{}); }
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseMove.AddDynamic(
			[this](const FOnMouseMove& EventArgs)->bool
			{
				InputEvents.emplace(InputEventTypes::FMouseMovement{
					.PrevCursorLocation = MouseCursorLocation,
					.CurrCursorLocation = EventArgs.CursorLocation });
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

void FInputContext::ProcessInput()
{
	if (IsValid())
	{
		while (!InputEvents.empty())
		{
			using namespace InputEventTypes;

			// @WIP: std::visit(..., InputEvents.front());
			InputEvents.pop();
		}
	}
}

ESwitchState FInputContext::GetKeyboardKeyState(EKeyboardKey Key) const noexcept
{
	return KeyboardFunctions::IsValidKey(Key)
		? KeyboardKeyStates.at(static_cast<std::size_t>(Key))
		: ESwitchState::Up;
}

ESwitchState FInputContext::GetMouseButtonState(EMouseButton Button) const noexcept
{
	return MouseFunctions::IsValidButton(Button)
		? MouseButtonStates.at(static_cast<std::size_t>(Button))
		: ESwitchState::Up;
}
