// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindowEvents.h"

#define DefineEventDispatcher(Event) , ##Event{}
#define DispatchEvent(Event) [this](const F##Event& EventArgs)->void { ##Event.Broadcast(EventArgs); },

FSystemWindowEvents::FSystemWindowEvents()
	: Queues{}
	, CurrentIndex{}
	, Mutex{}
	EnumerateSystemWindowEvents(DefineEventDispatcher)
{
}

void FSystemWindowEvents::Enqueue(FSystemWindowEvent&& Event)
{
	std::unique_lock<std::mutex> Lock{ Mutex };
	Queues[CurrentIndex].emplace(std::move(Event));
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
				EnumerateSystemWindowEvents(DispatchEvent) },
				Queue.front());
			Queue.pop();
		}
	}
}

#undef DefineEventDispatcher
#undef DispatchEvent
