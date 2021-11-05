// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include <Application/IApplication.h>
#include <Type/MulticastDelegate.h>

class FSystemWindow;

class FApplication final
	: public IApplication
{
public:
	explicit FApplication(FSystem& System);
	virtual ~FApplication() noexcept;

private:
	virtual bool Initialize(const FCommandLineArgs& CmdLine) noexcept override final;
	virtual void Terminate() noexcept override final;

	void Tick(FTimeDuration DeltaTime) override final;
	void Render(FTimeDuration DeltaTime) const override final;

private:
	std::unique_ptr<FSystemWindow> Window;
	FDelegateHandles WindowEvents;
};
