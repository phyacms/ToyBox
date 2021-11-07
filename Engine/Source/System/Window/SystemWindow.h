﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/String.h"
#include "SystemWindowEvents.h"

class ISystemWindowProcedure;

class FSystemWindow final
	: public TObject<FSystemWindow>
{
public:
	FSystemWindow(
		std::unique_ptr<ISystemWindowProcedure> WndProc,
		FStringView Title);
	~FSystemWindow() noexcept;

	FSystemWindow(const FSystemWindow&) = delete;
	FSystemWindow& operator=(const FSystemWindow&) & = delete;
	FSystemWindow(FSystemWindow&&) noexcept = delete;
	FSystemWindow& operator=(FSystemWindow&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;
	inline const FString& GetTitle() const noexcept { return Title; }

	void Present() noexcept;

	template<typename T>
	inline T& InterfaceAs() { return dynamic_cast<T&>(*WndProc); }

private:
	std::unique_ptr<ISystemWindowProcedure> WndProc;
	FString Title;

public:
	FSystemWindowEvents Events;
};
