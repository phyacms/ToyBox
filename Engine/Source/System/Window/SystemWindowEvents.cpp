// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindowEvents.h"

void FSystemWindowEvents::Enqueue(SystemWindowEvents::FEvent&& Event)
{
	std::visit(stdhelp::overloaded{
		[this](const auto&)->void {}, },
		Event);

	std::unique_lock<std::mutex> Lock{ Mutex };
	auto& Queue = Queues[CurrentIndex];
	Queue.emplace(std::move(Event));
}

void FSystemWindowEvents::Process()
{
	auto& Queue = Queues[CurrentIndex];
	if (!Queue.empty())
	{
		std::unique_lock<std::mutex> Lock{ Mutex };
		CurrentIndex = (CurrentIndex + 1) % Queues.size();
		Lock.unlock();

		while (!Queue.empty())
		{
			std::visit(stdhelp::overloaded{
				[](const auto&)->void {},
				[this](const SystemWindowEvents::FOnClosed&)->void { OnClosed.Broadcast(); }, },
				Queue.front());
			Queue.pop();
		}
	}
}
