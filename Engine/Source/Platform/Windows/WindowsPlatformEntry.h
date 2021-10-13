// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatform.h"
#include "System/CommandLineArgs.h"

class IApplication;

class FWindowsPlatformEntry final
{
public:
	explicit FWindowsPlatformEntry(HINSTANCE hInstance);
	virtual ~FWindowsPlatformEntry() noexcept;

public:
	std::int32_t Launch(IApplication& Application) noexcept;

private:
	static FCommandLineArgs ParseCommandLine() noexcept;

private:
	HINSTANCE hInstance;
	bool bCoInit;
	FCommandLineArgs CmdLine;
};

#endif
