// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/String.h"

class FSystemWindow;

class ISystemWindowProcedure
{
public:
	ISystemWindowProcedure();
	virtual ~ISystemWindowProcedure() noexcept;

	ISystemWindowProcedure(const ISystemWindowProcedure&) = delete;
	ISystemWindowProcedure& operator=(const ISystemWindowProcedure&) = delete;
	ISystemWindowProcedure(ISystemWindowProcedure&&) noexcept = delete;
	ISystemWindowProcedure& operator=(ISystemWindowProcedure&&) noexcept = delete;

public:
	inline bool IsValid() const noexcept { return Window != nullptr && IsValidImpl(); }
	bool Initialize(FSystemWindow& OwnerWindow) noexcept;
	void Terminate(FSystemWindow& OwnerWindow) noexcept;

	virtual void Present() noexcept = 0;

	FSystemWindow& GetWindow() noexcept { return *Window; }

private:
	virtual bool IsValidImpl() const noexcept = 0;
	virtual bool InitializeImpl(FSystemWindow& OwnerWindow) noexcept = 0;
	virtual void TerminateImpl(FSystemWindow& OwnerWindow) noexcept = 0;

private:
	FSystemWindow* Window;
};
