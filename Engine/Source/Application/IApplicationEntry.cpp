// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IApplicationEntry.h"
#include "IApplication.h"
#include "System/System.h"

IApplicationEntry::FEntryGuard::FEntryGuard(
	IApplicationEntry& Entry,
	IApplication& Application,
	const FCommandLineArgs& CmdLine)
	: Entry{ &Entry }
	, Application{ &Application }
	, bInit{}
{
	FLog::GetThreadLogger().SetIdentifier(USTR("MainThread"));

	bInit = !CmdLine.empty()
		&& Entry.Initialize()
		&& Application.Initialize(CmdLine);
	if (IsInitialized())
	{
		ReleaseLog(LogEngine) << USTR("Application initialized.") << LogEndl;
		Entry.BeginThreads();
	}
}

IApplicationEntry::FEntryGuard::~FEntryGuard() noexcept
{
	Entry->EndThreads();
	Application->Terminate();
	Entry->Terminate();
	ReleaseLog(LogEngine) << USTR("Application terminated.") << LogEndl;
}

IApplicationEntry::IApplicationEntry()
	: System{}
	, Application{}
	, bAbortThreads{}
	, GameThread{}
	, RenderThread{}
	, Mutex{}
{
}

IApplicationEntry::FEntryGuard IApplicationEntry::CreateEntryGuard(IApplication& Application)
{
	return { *this, Application, ParseCommandLine() };
}

void IApplicationEntry::BeginThreads()
{
	GameThread = std::thread(
		[this]()->void
		{
			FLog::GetThreadLogger().SetIdentifier(USTR("GameThread"));

			auto TickTime{ FTimeClock::now() };
			while (!bAbortThreads)
			{
				const auto Now{ FTimeClock::now() };
				const auto DeltaTime{ Now - TickTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / System.GetMaximumTickRate() })
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

			auto FrameTime{ FTimeClock::now() };
			while (!bAbortThreads)
			{
				const auto Now{ FTimeClock::now() };
				const auto DeltaTime{ Now - FrameTime };
				if (DeltaTime >= std::chrono::duration<double>{ 1.0 / System.GetMaximumFrameRate() })
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
