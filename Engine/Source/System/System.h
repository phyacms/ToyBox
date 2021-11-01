﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/String.h"

class FSystemWindow;

class FSystem final
{
public:
	FSystem() = default;
	~FSystem() noexcept = default;

	FSystem(const FSystem&) = delete;
	FSystem& operator=(const FSystem&) = delete;
	FSystem(FSystem&&) noexcept = delete;
	FSystem& operator=(FSystem&&) noexcept = delete;

public:
	void RequestAppExit(std::int32_t ExitCode) const noexcept;
	void ShowPopupMessage(FStringView Title, FStringView Content) const noexcept;

	[[nodiscard]] std::unique_ptr<FSystemWindow> CreateWindow(FStringView Title) const noexcept;
};