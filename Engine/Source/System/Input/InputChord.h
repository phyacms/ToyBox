// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "InputTrigger.h"

struct FInputChord final
{
public:
	FInputChord();
	FInputChord(const FInputChord&) = default;
	FInputChord& operator=(const FInputChord&) & = default;
	FInputChord(FInputChord&& Other) noexcept;
	FInputChord& operator=(FInputChord&& Other) & noexcept;
	~FInputChord() noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }

public:
	bool IsValid() const noexcept;
	inline void Reset() noexcept { Trigger = {}; Modifiers.InputCodes.clear(); }

	FInputChord& SetTrigger(FInputTrigger Trigger) noexcept;
	FInputChord& AddModifier(FInputCode InputCode) noexcept;
	FInputChord& RemoveModifier(FInputCode InputCode) noexcept;

	inline const FInputTrigger& GetTrigger() const noexcept { return Trigger; }
	inline const FInputCodes& GetModifiers() const noexcept { return Modifiers; }

private:
	FInputTrigger Trigger;
	FInputCodes Modifiers;
};
