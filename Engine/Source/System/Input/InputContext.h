// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "System/Window/SystemWindow.h"

class FInput;

class FInputContext final
{
public:
	FInputContext(
		FInput& Input,
		AObject<FSystemWindow>&& InputWindow);
	virtual ~FInputContext() noexcept;

	FInputContext(FInputContext&&) noexcept = delete;
	FInputContext& operator=(FInputContext&&) & noexcept = delete;
	FInputContext(const FInputContext&) = delete;
	FInputContext& operator=(const FInputContext&) & = delete;

public:
	bool IsValid() const noexcept;

protected:
	inline FSystemWindow& GetInputWindow() const noexcept { return *InputWindow.GetAddress(); }

private:
	FInput* Input;
	AObject<FSystemWindow> InputWindow;
};
