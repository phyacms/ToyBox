// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/String.h"

class ISystemWindowProcedure;

namespace PlatformFunctions
{
	void RequestAppExit(std::int32_t ExitCode) noexcept;
	void PrintDebugOutput(const FString& Message) noexcept;
	void ShowPopupMessage(const FString& Title, const FString& Content) noexcept;
	[[nodiscard]] std::unique_ptr<ISystemWindowProcedure> CreateWindowProcedure();

	std::optional<std::vector<char8_t>> StringToUTF8(const FString& Str);
	std::optional<FString> UTF8ToString(const std::vector<char8_t>& u8Chars);
}
