// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/TimePoint.h"
#include "System/CommandLineArgs.h"

class FSystem;

class IApplication
{
public:
	explicit IApplication(FSystem& System) : System{ &System } {}
	virtual ~IApplication() noexcept = default;

	IApplication(const IApplication&) = delete;
	IApplication& operator=(const IApplication&) = delete;
	IApplication(IApplication&&) noexcept = delete;
	IApplication& operator=(IApplication&&) noexcept = delete;

public:
	virtual bool Initialize(const FCommandLineArgs& CmdLine) noexcept = 0;
	virtual void Terminate() noexcept = 0;

	virtual void Tick(FTimeDuration DeltaTime) {}
	virtual void Render(FTimeDuration DeltaTime) {}

protected:
	inline FSystem& GetSystem() const noexcept { return *System; }

private:
	FSystem* System;
};
