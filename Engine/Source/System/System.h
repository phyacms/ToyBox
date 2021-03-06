// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/String.h"

class FSystemWindow;
class FInput;
class FGraphics;

class FSystem final
{
	friend class IApplicationEntry;

private:
	FSystem();
	~FSystem() noexcept;

	FSystem(const FSystem&) = delete;
	FSystem& operator=(const FSystem&) & = delete;
	FSystem(FSystem&&) noexcept = delete;
	FSystem& operator=(FSystem&&) & noexcept = delete;

public:
	void RequestAppExit(std::int32_t ExitCode) const noexcept;
	void PrintDebugOutput(const FString& Message) const noexcept;
	void ShowPopupMessage(const FString& Title, const FString& Content) const noexcept;

	[[nodiscard]] std::unique_ptr<FSystemWindow> CreateWindow(FString Title) const& noexcept;
	FInput& GetInput() & noexcept;
	FGraphics& GetGraphics() & noexcept;

	inline double GetMaximumTickRate() const noexcept { return MaximumTickRate; }
	inline double GetMaximumFrameRate() const noexcept { return MaximumFrameRate; }

private:
	std::unique_ptr<FInput> Input;
	std::unique_ptr<FGraphics> Graphics;

	// @TODO: Engine configuration.
	double MaximumTickRate;
	double MaximumFrameRate;
};
