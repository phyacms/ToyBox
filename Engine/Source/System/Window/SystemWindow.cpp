// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindow.h"
#include "ISystemWindowProcedure.h"

#define DefineEventDispatcher(Event) , ##Event{}
#define DispatchEvent(Event) [this](const SystemWindowEventTypes::F##Event& EventArgs)->void { ##Event.Broadcast(EventArgs); },

FSystemWindow::FSystemWindowEvents::FSystemWindowEvents()
	: Queues{}
	, CurrentIndex{}
	, Mutex{}
	EnumerateSystemWindowEvents(DefineEventDispatcher)
{
}

void FSystemWindow::FSystemWindowEvents::Enqueue(SystemWindowEventTypes::FEvent&& Event)
{
	std::unique_lock<std::mutex> Lock{ Mutex };
	Queues[CurrentIndex].emplace(std::move(Event));
}

void FSystemWindow::FSystemWindowEvents::Process()
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

FSystemWindow::FSystemWindow(std::unique_ptr<ISystemWindowProcedure> WndProc, FString Title)
	: TObject<FSystemWindow>(*this)
	, WndProc{ std::move(WndProc) }
	, Title{ std::move(Title) }
	, Events{}
{
	if (this->WndProc != nullptr)
	{
		this->WndProc->Initialize(*this);
	}
}

FSystemWindow::~FSystemWindow() noexcept
{
	if (WndProc != nullptr)
	{
		WndProc->Terminate(*this);
	}
}

bool FSystemWindow::IsValid() const noexcept
{
	return WndProc != nullptr
		&& WndProc->IsValid()
		&& &WndProc->GetWindow() == this;
}

FScreenArea FSystemWindow::GetWindowArea() const noexcept
{
	return IsValid()
		? WndProc->GetWindowArea()
		: FScreenArea{};
}

FScreenArea FSystemWindow::GetClientArea() const noexcept
{
	return IsValid()
		? WndProc->GetClientArea()
		: FScreenArea{};
}

void FSystemWindow::Present() noexcept
{
	if (IsValid())
	{
		WndProc->Present();
	}
}

void FSystemWindow::Close() noexcept
{
	if (IsValid())
	{
		WndProc->Close();
	}
}
