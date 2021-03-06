// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/System.h"
#include "System/CommandLineArgs.h"

class IApplication;

class IApplicationEntry
{
public:
	IApplicationEntry();
	virtual ~IApplicationEntry() noexcept = default;

	IApplicationEntry(const IApplicationEntry&) = delete;
	IApplicationEntry& operator=(const IApplicationEntry&) & = delete;
	IApplicationEntry(IApplicationEntry&&) noexcept = delete;
	IApplicationEntry& operator=(IApplicationEntry&&) & noexcept = delete;

public:
	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_base_of_v<IApplication, T>>>
	inline [[nodiscard]] std::int32_t Launch(Ts&&... Params)
	{
		if (Application != nullptr)
		{
			return EXIT_FAILURE;
		}
		Application = std::make_unique<T>(System, std::forward<Ts>(Params)...);
		return AppMain(*Application);
	}

protected:
	[[nodiscard]] struct FEntryGuard final
	{
		friend IApplicationEntry;

	private:
		FEntryGuard(
			IApplicationEntry& Entry,
			IApplication& Application,
			const FCommandLineArgs& CmdLine);

	public:
		~FEntryGuard() noexcept;

	public:
		inline bool IsInitialized() const noexcept { return bInit; }

	private:
		IApplicationEntry* Entry;
		IApplication* Application;
		bool bInit;
	}
	CreateEntryGuard(IApplication& Application);

private:
	virtual bool Initialize() noexcept = 0;
	virtual void Terminate() noexcept = 0;

	virtual FCommandLineArgs ParseCommandLine() const noexcept = 0;
	virtual std::int32_t AppMain(IApplication& Application) = 0;

	void BeginThreads();
	void EndThreads() noexcept;

private:
	FSystem System;
	std::unique_ptr<IApplication> Application;

	std::atomic_bool bAbortThreads;
	std::thread GameThread;
	std::thread RenderThread;
	std::mutex Mutex;
};

#endif
