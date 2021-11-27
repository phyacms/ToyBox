// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputContext.h"
#include "System/Window/SystemWindow.h"
#include "Controller/IInputActionController.h"

FInputContext::FInputContext(
	FInput& Input,
	FSystemWindow& InputWindow)
	: TObject<FInputContext>(*this)
	, Input{ &Input }
	, InputWindow{ InputWindow }
	, KeyboardMouseEvents{}
	, KeyboardKeyStates{}
	, MouseButtonStates{}
	, MouseCursorLocation{}
	, ControllerIdIssuer{}
	, Controllers{}
{
	if (this->InputWindow.IsValid())
	{
		auto& Events{ this->InputWindow->Events };
		KeyboardMouseEvents += Events.OnKeyboardKey.AddDynamic(*this, &FInputContext::SetKeyboardKeyState);
		KeyboardMouseEvents += Events.OnMouseButton.AddDynamic(*this, &FInputContext::SetMouseButtonState);
		KeyboardMouseEvents += Events.OnMouseWheel.AddDynamic(*this, &FInputContext::ConsumeMouseWheel);
		KeyboardMouseEvents += Events.OnMouseMove.AddDynamic(*this, &FInputContext::ConsumeMouseMove);
	}
}

FInputContext::~FInputContext() noexcept
{
	Controllers.clear();
	KeyboardMouseEvents.Clear();
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

AInputControllerBinding FInputContext::BindInputController(IInputController& Controller)
{
	AUniqueId Issued{ ControllerIdIssuer.Issue() };
	Controllers.emplace_front(std::make_pair(Issued.GetHash(), AObject<IInputController>{ Controller }));
	return { *this, std::move(Issued) };
}

AInputControllerBinding FInputContext::BindInputController(IInputActionController& ActionController)
{
	ActionController.BindStatics();
	return BindInputController(reinterpret_cast<IInputController&>(ActionController));
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

bool FInputContext::SetKeyboardKeyState(const FOnKeyboardKey& EventArgs)
{
	if (InputFunctions::IsValidKey(EventArgs.Key))
	{
		const auto Index{ InputFunctions::ToIndex(EventArgs.Key) };
		const auto Event{ ::ToSwitchEvent(GetKeyboardKeyState(EventArgs.Key), EventArgs.State) };
		KeyboardKeyStates[Index] = EventArgs.State;
		if (Event != ESwitchEvent::Idle)
		{
			for (auto& [Id, Controller] : Controllers)
			{
				if (Controller.IsValid()
					&& Controller->DispatchInputAction(
						*this,
						EventArgs.Time,
						FInputCodeTrigger{
							.InputCode{ EventArgs.Key },
							.Event{ Event } }))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInputContext::SetMouseButtonState(const FOnMouseButton& EventArgs)
{
	if (InputFunctions::IsValidButton(EventArgs.Button))
	{
		const auto Index{ InputFunctions::ToIndex(EventArgs.Button) };
		const auto Event{ ::ToSwitchEvent(GetMouseButtonState(EventArgs.Button), EventArgs.State) };
		MouseButtonStates[Index] = EventArgs.State;
		if (Event != ESwitchEvent::Idle)
		{
			for (auto& [Id, Controller] : Controllers)
			{
				if (Controller.IsValid()
					&& Controller->DispatchInputAction(
						*this,
						EventArgs.Time,
						FInputCodeTrigger{
							.InputCode{ EventArgs.Button },
							.Event{ Event } }))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInputContext::ConsumeMouseWheel(const FOnMouseWheel& EventArgs)
{
	for (auto& [Id, Controller] : Controllers)
	{
		bool bHandled{};
		for (auto Wheel : EventArgs.WheelDelta)
		{
			bHandled |=
				Controller.IsValid()
				&& Controller->DispatchInputAction(
					*this,
					EventArgs.Time,
					Wheel);
		}
		if (bHandled)
		{
			return true;
		}
	}
	return false;
}

bool FInputContext::ConsumeMouseMove(const FOnMouseMove& EventArgs)
{
	const auto& Displacement{ EventArgs.CursorLocation - MouseCursorLocation };
	MouseCursorLocation = EventArgs.CursorLocation;
	for (auto& [Id, Controller] : Controllers)
	{
		if (Controller.IsValid()
			&& Controller->DispatchMouseMovement(*this, EventArgs.Time, Displacement))
		{
			return true;
		}
	}
	return false;
}
