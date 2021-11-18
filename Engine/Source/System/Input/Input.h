// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

class FSystem;
class FSystemWindow;
class FInputContext;

class FInput final
{
public:
	explicit FInput(FSystem& System);
	~FInput() noexcept;

	FInput(const FInput&) = delete;
	FInput& operator=(const FInput&) & = delete;
	FInput(FInput&&) noexcept = delete;
	FInput& operator=(FInput&&) & noexcept = delete;

public:
	[[nodiscard]] std::unique_ptr<FInputContext> CreateContext(FSystemWindow& OutputWindow) &;

private:
	FSystem* System;
};
