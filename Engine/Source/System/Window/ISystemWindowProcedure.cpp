// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "ISystemWindowProcedure.h"

ISystemWindowProcedure::ISystemWindowProcedure()
	: Window{ nullptr }
{
}

ISystemWindowProcedure::~ISystemWindowProcedure() noexcept = default;

bool ISystemWindowProcedure::Initialize(FSystemWindow& OwnerWindow) noexcept
{
	Window = &OwnerWindow;
	return InitializeImpl(OwnerWindow);
}

void ISystemWindowProcedure::Terminate(FSystemWindow& OwnerWindow) noexcept
{
	TerminateImpl(OwnerWindow);
}
