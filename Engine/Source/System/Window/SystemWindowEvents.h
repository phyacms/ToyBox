// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/SwitchState.h"
#include "Type/EventDispatcher.h"
#include "System/Input/InputCode.h"

namespace SystemWindowEvents
{
	struct FOnClosed final {};
	struct FOnResized final { std::uint32_t Width{}; std::uint32_t Height{}; };

	struct FOnKeyboardKey final { EKeyboardKey Key{}; ESwitchState State{}; };
	struct FOnMouseButton final { EMouseButton Button{}; ESwitchState State{}; };
	struct FOnMouseWheel final { std::int32_t dWheel{}; };
	struct FOnMouseMove final { std::int32_t X{}; std::int32_t Y{}; };

	using FEvent = std::variant<
		FOnClosed,
		FOnResized,
		FOnKeyboardKey,
		FOnMouseButton,
		FOnMouseWheel,
		FOnMouseMove>;
}

class FSystemWindowEvents final
{
private:
	using FEvent = SystemWindowEvents::FEvent;

public:
	FSystemWindowEvents();
	~FSystemWindowEvents() noexcept = default;

	FSystemWindowEvents(const FSystemWindowEvents&) = delete;
	FSystemWindowEvents& operator=(const FSystemWindowEvents&) & = delete;
	FSystemWindowEvents(FSystemWindowEvents&&) noexcept = delete;
	FSystemWindowEvents& operator=(FSystemWindowEvents&&) & noexcept = delete;

public:
	void Enqueue(FEvent&& Event);
	void Process();

private:
	std::array<std::queue<FEvent>, 2> Queues;
	std::size_t CurrentIndex;
	std::mutex Mutex;

public:
	TEventDispatcher<bool(void)> OnClosed;
	TEventDispatcher<bool(std::uint32_t, std::uint32_t)> OnResized;
	TEventDispatcher<bool(EKeyboardKey, ESwitchState)> OnKeyboardKey;
	TEventDispatcher<bool(EMouseButton, ESwitchState)> OnMouseButton;
	TEventDispatcher<bool(std::int32_t)> OnMouseWheel;
	TEventDispatcher<bool(std::int32_t, std::int32_t)> OnMouseMove;
};
