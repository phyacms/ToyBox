﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/String/String.h"

class IPlatform
{
public:
	IPlatform() = default;
	virtual ~IPlatform() noexcept = default;

	IPlatform(const IPlatform&) = delete;
	IPlatform& operator=(const IPlatform&) = delete;
	IPlatform(IPlatform&&) noexcept = delete;
	IPlatform& operator=(IPlatform&&) noexcept = delete;

public:
	virtual void RequestAppExit(std::int32_t ExitCode) noexcept = 0;
	virtual void ShowPopupMessage(FStringView Title, FStringView Content) noexcept = 0;
};
