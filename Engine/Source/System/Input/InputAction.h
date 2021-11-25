// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Delegate.h"
#include "Type/Container.h"
#include "InputChord.h"

class FInputContext;

struct FInputAction final
{
	using CallbackFn = TDelegate<bool(const FInputContext&)>;

	FInputChord Chord{};
	CallbackFn Callback{};

	inline bool IsValid() const noexcept { return Callback.IsBound() && Chord.IsValid(); }
	inline void Clear() noexcept { Chord.Reset(); Callback.Unbind(); }
};

class FInputActionBindings final
{
private:
	using ContainerType = std::vector<FInputAction>;

public:
	inline FInputActionBindings& operator+=(const FInputAction& Action) { if (Action.IsValid()) { Actions.emplace_back(Action); } return *this; }
	inline FInputActionBindings& operator+=(FInputAction&& Action) { if (Action.IsValid()) { Actions.emplace_back(std::move(Action)); } return *this; }

public:
	EnumerateConstIterators(Actions, ExposeConstIterators)

public:
	inline bool IsEmpty() const noexcept { return Actions.empty(); }
	inline void Clear() noexcept { Actions.clear(); }

private:
	std::vector<FInputAction> Actions{};
};
