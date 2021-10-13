﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "System/CommandLineArgs.h"

class IApplication
{
public:
	IApplication() = default;
	virtual ~IApplication() noexcept = default;

	IApplication(const IApplication&) = delete;
	IApplication& operator=(const IApplication&) = delete;
	IApplication(IApplication&&) noexcept = delete;
	IApplication& operator=(IApplication&&) noexcept = delete;

public:
	virtual bool Initialize(const FCommandLineArgs& CmdLine) noexcept = 0;
	virtual void Terminate() noexcept = 0;
};
