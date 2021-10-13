﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <Platform/Platform.h>

FApplication::FApplication()
{
}

FApplication::~FApplication() noexcept
{
}

bool FApplication::Initialize(const FCommandLineArgs& CmdLine) noexcept
{
	IPlatform& Platform{ FPlatform::GetInstance() };
	Platform.ShowPopupMessage(USTR("ProjectTemplate"), USTR("Hello, World!"));
	Platform.RequestAppExit(EXIT_SUCCESS);
	return true;
}

void FApplication::Terminate() noexcept
{
}
