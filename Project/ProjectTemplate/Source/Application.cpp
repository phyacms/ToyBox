// Copyrights 2021 by phyacms. All Rights Reserved.

#include "stdafx.h"
#include "Application.h"
#include <System/System.h>

FApplication::FApplication(FSystem& System)
	: IApplication(System)
{
}

FApplication::~FApplication() noexcept
{
}

bool FApplication::Initialize(const FCommandLineArgs& CmdLine) noexcept
{
	FSystem& System{ GetSystem() };
	System.ShowPopupMessage(USTR(PROJECT_NAME), USTR(PROJECT_COPYRIGHT));
	System.RequestAppExit(EXIT_SUCCESS);
	return true;
}

void FApplication::Terminate() noexcept
{
}
