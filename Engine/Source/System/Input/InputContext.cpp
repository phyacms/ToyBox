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
	, InputEvents{}
	, Issuer{}
	, Controllers{}
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
						InputEvents.emplace(
							FPulseInput{
								.Event{ static_cast<EPulseInput>(Event) },
								.InputCode{ EventArgs.Key } });
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
						InputEvents.emplace(
							FPulseInput{
								.Event{ static_cast<EPulseInput>(Event) },
								.InputCode{ EventArgs.Button } });
					}
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseWheel.AddDynamic(
			[this](const FOnMouseWheel& EventArgs)->bool
			{
				auto WheelDelta{ EventArgs.WheelDelta };
				while (WheelDelta-- > 0)
				{
					InputEvents.emplace(
						FPulseInput{
							.Event{ EPulseInput::RolledUp },
							.InputCode{ EMouseButton::Middle } });
				}
				while (WheelDelta++ < 0)
				{
					InputEvents.emplace(
						FPulseInput{
							.Event{ EPulseInput::RolledDown },
							.InputCode{ EMouseButton::Middle } });
				}
				return false;
			});

		KeyboardMouseEvents += this->InputWindow->Events.OnMouseMove.AddDynamic(
			[this](const FOnMouseMove& EventArgs)->bool
			{
				InputEvents.emplace(
					FMouseMovement{
						.CursorLocation = EventArgs.CursorLocation });
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
