// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "SwitchState.h"
#include "InputCode.h"

enum class EPulseInput : std::size_t
{
	Idle     = static_cast<std::size_t>(ESwitchEvent::Idle),
	Pressed  = static_cast<std::size_t>(ESwitchEvent::Pressed),
	Released = static_cast<std::size_t>(ESwitchEvent::Released),
	Repeated = static_cast<std::size_t>(ESwitchEvent::Repeated),
	RolledDown,
	RolledUp,
};

struct FPulseInput final
{
	EPulseInput Event{};
	FInputCode InputCode{};
	inline operator bool() const noexcept { return IsValid(); }
	bool IsValid() const noexcept;
};

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
	inline bool IsValid() const noexcept { return Trigger.IsValid(); }
	inline void Reset() noexcept { Trigger = FPulseInput{}; Modifiers.Values.clear(); }

	inline FInputChord& SetEvent(EPulseInput Event) noexcept { Trigger.Event = Event; return *this; }
	inline FInputChord& SetTrigger(FInputCode InputCode) noexcept { Trigger.InputCode = std::move(InputCode); return *this; }
	inline FInputChord& AddModifier(FInputCode InputCode) { if (InputCode.IsValid()) { Modifiers += InputCode; } return *this; }
	inline FInputChord& RemoveModifier(FInputCode InputCode) noexcept { if (InputCode.IsValid()) { Modifiers -= InputCode; } return *this; }

	inline EPulseInput GetEvent() const noexcept { return Trigger.Event; }
	inline FInputCode GetTrigger() const noexcept { return Trigger.InputCode; }
	inline const FInputCodes& GetModifiers() const noexcept { return Modifiers; }

private:
	FPulseInput Trigger;
	FInputCodes Modifiers;
};
