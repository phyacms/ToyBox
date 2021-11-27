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
	, ControllerIdIssuer{}
	, Controllers{}
{
	if (this->InputWindow.IsValid())
	{
		using namespace SystemWindowEventTypes;

		KeyboardMouseEvents += this->InputWindow->Events.OnKeyboardKey.AddDynamic(
			[this](const FOnKeyboardKey& EventArgs)->bool { return SetKeyboardKeyState(EventArgs.Key, EventArgs.State); });

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseButton.AddDynamic(
			[this](const FOnMouseButton& EventArgs)->bool { return SetMouseButtonState(EventArgs.Button, EventArgs.State); });

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseWheel.AddDynamic(
			[this](const FOnMouseWheel& EventArgs)->bool { return ConsumeMouseWheelMove(EventArgs.WheelDelta); });

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseMove.AddDynamic(
			[this](const FOnMouseMove& EventArgs)->bool {
				SetMouseCursorLocation(EventArgs.CursorLocation);
				return false; });
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

AInputControllerBinding FInputContext::BindInputController(AObject<IInputController>&& Controller)
{
	if (!Controller.IsValid())
	{
		return {};
	}

	AUniqueId Issued{ ControllerIdIssuer.Issue() };
	Controllers.emplace_front(std::make_pair(Issued.GetHash(), std::move(Controller)));
	return { *this, std::move(Issued) };
}

AInputControllerBinding FInputContext::BindInputController(IInputActionController& ActionController)
{
	ActionController.BindStatics();
	return BindInputController(AObject<IInputController>{ ActionController });
}

void FInputContext::UnbindInputController(AInputControllerBinding& Handle) noexcept
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

bool FInputContext::SetKeyboardKeyState(EKeyboardKey Key, ESwitchState State)
{
	if (InputFunctions::IsValidKey(Key))
	{
		const auto Index{ InputFunctions::ToIndex(Key) };
		const auto Event{ ::ToSwitchEvent(GetKeyboardKeyState(Key), State) };
		KeyboardKeyStates[Index] = State;
		if (Event != ESwitchEvent::Idle)
		{
			for (auto& [Id, Controller] : Controllers)
			{
				if (Controller.IsValid()
					&& Controller->DispatchKeyboardKeyEvent(*this, Key, Event))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInputContext::SetMouseButtonState(EMouseButton Button, ESwitchState State)
{
	if (InputFunctions::IsValidButton(Button))
	{
		const auto Index{ InputFunctions::ToIndex(Button) };
		const auto Event{ ::ToSwitchEvent(GetMouseButtonState(Button), State) };
		MouseButtonStates[Index] = State;
		if (Event != ESwitchEvent::Idle)
		{
			for (auto& [Id, Controller] : Controllers)
			{
				if (Controller.IsValid()
					&& Controller->DispatchMouseButtonEvent(*this, Button, Event))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInputContext::ConsumeMouseWheelMove(const FMouseWheelTriggers& WheelMove)
{
	for (auto& [Id, Controller] : Controllers)
	{
		bool bHandled{};
		for (auto dWheel : WheelMove)
		{
			bHandled |=
				Controller.IsValid()
				&& Controller->DispatchMouseWheelMoveEvent(*this, dWheel);
		}
		if (bHandled)
		{
			return true;
		}
	}
	return false;
}

void FInputContext::SetMouseCursorLocation(const FScreenLocation& CursorLocation)
{
	MouseCursorLocation = CursorLocation;
}
