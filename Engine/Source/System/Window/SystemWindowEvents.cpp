// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindowEvents.h"

using namespace SystemWindowEvents;

FSystemWindowEvents::FSystemWindowEvents()
	: Queues{}
	, CurrentIndex{}
	, Mutex{}
	, OnClosed{}
	, OnResized{}
{
}

void FSystemWindowEvents::Enqueue(FEvent&& Event)
{
	std::unique_lock<std::mutex> Lock{ Mutex };

	bool bEnqueue{ true };
	auto& Queue = Queues[CurrentIndex];

	std::visit(stdhelp::overloaded{
		[&](const auto&)->void {},
		[&](const FOnResized& Event)->void
		{
			if (!Queue.empty())
			{
				if (auto Ptr{ std::get_if<FOnResized>(&Queue.back()) };
					Ptr != nullptr)
				{
					*Ptr = Event;
					bEnqueue = false;
				}
			}
		}, },
		Event);

	if (bEnqueue)
	{
		Queue.emplace(std::move(Event));
	}
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
				[this](const FOnClosed&)->void { OnClosed.Broadcast(); },
				[this](const FOnResized& EventArgs)->void { OnResized.Broadcast(EventArgs.Width, EventArgs.Height); }, },
				Queue.front());
			Queue.pop();
		}
	}
}
