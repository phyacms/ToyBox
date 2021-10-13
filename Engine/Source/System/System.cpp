// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "System.h"
#include <Platform/Platform.h>

void FSystem::RequestAppExit(std::int32_t ExitCode) const noexcept { FPlatform::GetInterface().RequestAppExit(ExitCode); }
void FSystem::ShowPopupMessage(FStringView Title, FStringView Content) const noexcept { FPlatform::GetInterface().ShowPopupMessage(Title, Content); }
