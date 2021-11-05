// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/EventDispatcher.h"

namespace SystemWindowEvents
{
	struct FOnClosed final {};
	using FEvent = std::variant<FOnClosed>;
}

class FSystemWindowEvents final
{
public:
	FSystemWindowEvents() : Queues{}, CurrentIndex{}, Mutex{} {}
	~FSystemWindowEvents() noexcept = default;

	FSystemWindowEvents(const FSystemWindowEvents&) = delete;
	FSystemWindowEvents& operator=(const FSystemWindowEvents&) = delete;
	FSystemWindowEvents(FSystemWindowEvents&&) noexcept = delete;
	FSystemWindowEvents& operator=(FSystemWindowEvents&&) noexcept = delete;

public:
	void Enqueue(SystemWindowEvents::FEvent&& Event);
	void Process();

public:
	TEventDispatcher<bool(void)> OnClosed;

private:
	std::array<std::queue<SystemWindowEvents::FEvent>, 2> Queues;
	std::size_t CurrentIndex;
	std::mutex Mutex;
};
