// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindowEvents.h"

void FSystemWindowEvents::Enqueue(SystemWindowEvents::FEvent&& Event)
{
	Queue.emplace(std::move(Event));
}

void FSystemWindowEvents::Process()
{
	while (!Queue.empty())
	{
		std::visit(stdhelp::overloaded{
			[this](const auto& EventArgs)->void { OnClosed.Broadcast(EventArgs); }, },
			Queue.front());
		Queue.pop();
	}
}
