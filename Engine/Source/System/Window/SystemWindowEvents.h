// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/EventDispatcher.h"

namespace SystemWindowEvents
{
	struct FOnClosed final {};
	struct FOnResized final { std::uint32_t Width{}; std::uint32_t Height{}; };
	using FEvent = std::variant<FOnClosed, FOnResized>;
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
};
