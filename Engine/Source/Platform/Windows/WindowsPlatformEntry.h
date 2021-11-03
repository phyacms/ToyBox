// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatform.h"
#include "System/System.h"
#include "System/CommandLineArgs.h"

class IApplication;

class FWindowsPlatformEntry final
{
public:
	explicit FWindowsPlatformEntry(HINSTANCE hInstance);
	~FWindowsPlatformEntry() noexcept;

	FWindowsPlatformEntry(const FWindowsPlatformEntry&) = delete;
	FWindowsPlatformEntry& operator=(const FWindowsPlatformEntry&) = delete;
	FWindowsPlatformEntry(FWindowsPlatformEntry&&) noexcept = delete;
	FWindowsPlatformEntry& operator=(FWindowsPlatformEntry&&) noexcept = delete;

public:
	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_base_of_v<IApplication, T>>>
	inline std::int32_t Launch(Ts&&... Params)
	{
		FSystem System{};
		auto Application{ std::make_unique<T>(System, std::forward<Ts>(Params)...) };
		return Launch(System, *Application);
	}

private:
	static FCommandLineArgs ParseCommandLine() noexcept;

	std::int32_t Launch(FSystem& System, IApplication& Application);

private:
	bool bCoInit;
	FCommandLineArgs CmdLine;
};

#endif
