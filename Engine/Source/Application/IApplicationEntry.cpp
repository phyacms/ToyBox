// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IApplicationEntry.h"
#include "IApplication.h"

IApplicationEntry::FEntryGuard::FEntryGuard(
	IApplicationEntry& Entry,
	IApplication& Application,
	const FCommandLineArgs& CmdLine)
	: Entry{ &Entry }
	, Application{ &Application }
	, bInit{ !CmdLine.empty() && Application.Initialize(CmdLine) }
{
	if (IsInitialized())
	{
		Entry.BeginThreads();
	}
}

IApplicationEntry::FEntryGuard::~FEntryGuard() noexcept
{
	Entry->EndThreads();
	Application->Terminate();
}

IApplicationEntry::IApplicationEntry()
	: System{}
	, Application{}
	, bAbortThreads{}
	, GameThread{}
	, RenderThread{}
	, Mutex{}
	, TickRate{ 100000.0 }
	, FrameRate{ 300.0 }
{
}

IApplicationEntry::FEntryGuard IApplicationEntry::CreateEntryGuard(IApplication& Application)
{
	return FEntryGuard{ *this, Application, ParseCommandLine() };
}

void IApplicationEntry::BeginThreads()
{
	GameThread = std::thread(
		[this]()->void
		{
			auto TickTime{ System.PreciseNow() };
			while (!bAbortThreads)
			{
				const auto Now{ System.PreciseNow() };
				const auto DeltaTime{ Now - TickTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / FrameRate })
				{
					std::unique_lock<std::mutex> Lock{ Mutex, std::defer_lock };
					if (Lock.try_lock())
					{
						Application->Tick(Now, DeltaTime);
						Lock.unlock();
					}
					TickTime = System.PreciseNow();
				}
			}
		});

	RenderThread = std::thread(
		[this]()->void
		{
			auto RenderTime{ System.PreciseNow() };
			while (!bAbortThreads)
			{
				const auto Now{ System.PreciseNow() };
				const auto DeltaTime{ Now - RenderTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / FrameRate })
				{
					std::unique_lock<std::mutex> Lock{ Mutex };
					if (!bAbortThreads)
					{
						Application->Render(Now, DeltaTime);
						Lock.unlock();
					}
					RenderTime = System.PreciseNow();
				}
			}
		});
}

void IApplicationEntry::EndThreads() noexcept
{
	bAbortThreads = true;
	if (GameThread.joinable()) { GameThread.join(); }
	if (RenderThread.joinable()) { RenderThread.join(); }
	bAbortThreads = false;
}
