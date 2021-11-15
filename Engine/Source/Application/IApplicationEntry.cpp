// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IApplicationEntry.h"
#include "IApplication.h"
#include "Log/Log.h"

IApplicationEntry::FEntryGuard::FEntryGuard(
	IApplicationEntry& Entry,
	IApplication& Application,
	const FCommandLineArgs& CmdLine)
	: Entry{ &Entry }
	, Application{ &Application }
	, bInit{}
{
	FLog::GetThreadLogger().SetIdentifier(USTR("MainThread"));

	bInit = !CmdLine.empty() && Application.Initialize(CmdLine);
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
			FLog::GetThreadLogger().SetIdentifier(USTR("GameThread"));

			auto TickTime{ System.PreciseNow() };
			while (!bAbortThreads)
			{
				const auto Now{ System.PreciseNow() };
				const auto DeltaTime{ Now - TickTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / TickRate })
				{
					std::unique_lock<std::mutex> Lock(Mutex, std::defer_lock);
					if (Lock.try_lock())
					{
						Application->Tick(DeltaTime);
						Lock.unlock();

						TickTime = Now;
					}
					std::this_thread::sleep_for(std::chrono::nanoseconds{ 10 });
				}
			}
		});

	RenderThread = std::thread(
		[this]()->void
		{
			FLog::GetThreadLogger().SetIdentifier(USTR("RenderThread"));

			auto FrameTime{ System.PreciseNow() };
			while (!bAbortThreads)
			{
				const auto Now{ System.PreciseNow() };
				const auto DeltaTime{ Now - FrameTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / FrameRate })
				{
					std::unique_lock<std::mutex> Lock(Mutex);
					if (!bAbortThreads)
					{
						Application->Render(DeltaTime);
						Lock.unlock();
					}
					FrameTime = Now;
					std::this_thread::sleep_for(std::chrono::nanoseconds{ 10 });
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
