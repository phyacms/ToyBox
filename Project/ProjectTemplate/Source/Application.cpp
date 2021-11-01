// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>

FApplication::FApplication()
{
}

FApplication::~FApplication() noexcept
{
}

bool FApplication::Initialize(FSystem& System, const FCommandLineArgs& CmdLine) noexcept
{
	System.ShowPopupMessage(USTR(PROJECT_NAME), USTR(PROJECT_COPYRIGHT));
	System.RequestAppExit(EXIT_SUCCESS);
	return true;
}

void FApplication::Terminate(FSystem& System) noexcept
{
}
