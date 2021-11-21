// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatform.h"
#include "Application/IApplicationEntry.h"

class IApplication;

class FWindowsPlatformEntry final
	: public IApplicationEntry
{
public:
	explicit FWindowsPlatformEntry(HINSTANCE hInstance);
	virtual ~FWindowsPlatformEntry() noexcept;

private:
	virtual bool Initialize() noexcept override final;
	virtual void Terminate() noexcept override final;

	virtual FCommandLineArgs ParseCommandLine() const noexcept override final;
	virtual std::int32_t AppMain(IApplication& Application) override final;

private:
	bool bCoInit;
};

#endif
