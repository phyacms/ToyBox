// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/TimePoint.h"
#include "Type/String.h"

class FSystemWindow;
class FGraphics;

class FSystem final
{
public:
	FSystem();
	~FSystem() noexcept;

	FSystem(const FSystem&) = delete;
	FSystem& operator=(const FSystem&) = delete;
	FSystem(FSystem&&) noexcept = delete;
	FSystem& operator=(FSystem&&) noexcept = delete;

public:
	inline FTimePoint PreciseNow() const noexcept { return FTimeClock::now(); }

	void RequestAppExit(std::int32_t ExitCode) const noexcept;
	void PrintDebugOutput(FStringView Message) const noexcept;
	void ShowPopupMessage(FStringView Title, FStringView Content) const noexcept;

	[[nodiscard]] std::unique_ptr<FSystemWindow> CreateWindow(FStringView Title) const noexcept;
	FGraphics& GetGraphics() noexcept;

private:
	std::unique_ptr<FGraphics> Graphics;
};
