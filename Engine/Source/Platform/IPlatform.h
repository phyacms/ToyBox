// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/String.h"

class ISystemWindowProcedure;

class IPlatform
{
public:
	IPlatform() = default;
	virtual ~IPlatform() noexcept = default;

	IPlatform(const IPlatform&) = delete;
	IPlatform& operator=(const IPlatform&) & = delete;
	IPlatform(IPlatform&&) noexcept = delete;
	IPlatform& operator=(IPlatform&&) & noexcept = delete;

private:
	friend class FSystem;
	virtual void RequestAppExit(std::int32_t ExitCode) const noexcept = 0;
	virtual void PrintDebugOutput(FStringView Message) const noexcept = 0;
	virtual void ShowPopupMessage(FStringView Title, FStringView Content) const noexcept = 0;
	virtual [[nodiscard]] std::unique_ptr<ISystemWindowProcedure> CreateWindowProcedure() const noexcept = 0;

private:
	friend class FString;
	virtual std::optional<std::vector<char8_t>> StringToUTF8(const FString& Str) const = 0;
	virtual std::optional<FString> UTF8ToString(const std::vector<char8_t>& u8Chars) const = 0;
};
