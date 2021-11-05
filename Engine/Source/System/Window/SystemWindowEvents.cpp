// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindowEvents.h"

void FSystemWindowEvents::Enqueue(SystemWindowEvents::FEvent&& Event)
{
	std::lock_guard<std::mutex> Lock{ Mutex };
	std::visit(stdhelp::overloaded{
		[this](const auto&)->void {}, },
		Event);
	Queue.emplace(std::move(Event));
}

void FSystemWindowEvents::Process()
{
	std::lock_guard<std::mutex> Lock{ Mutex };
	while (!Queue.empty())
	{
		std::visit(stdhelp::overloaded{
			[this](const auto& EventArgs)->void { OnClosed.Broadcast(EventArgs); }, },
			Queue.front());
		Queue.pop();
	}
}
