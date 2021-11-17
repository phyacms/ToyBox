// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Object.h"
#include "Type/UniqueId.h"
#include "InputAction.h"

// @TODO: Modify to bind/unbind actions dynamically.
class IInputController
	: public TObject<IInputController>
{
private:
	struct FBindings;

	using Iterator = FInputActionBindings::Iterator;
	using ConstIterator = FInputActionBindings::ConstIterator;

	using IteratorPair = FInputActionBindings::IteratorPair;
	using ConstIteratorPair = FInputActionBindings::ConstIteratorPair;

public:
	IInputController() : TObject<IInputController>(*this), Actions{} {}
	virtual ~IInputController() noexcept = default;

	IInputController(const IInputController&) = delete;
	IInputController& operator=(const IInputController&) & = delete;
	IInputController(IInputController&&) noexcept = delete;
	IInputController& operator=(IInputController&&) & noexcept = delete;

public:
	inline void BindStatics() {
		std::call_once(
			Actions.first,
			[this]()->void { BindInputActions(Actions.second); }); }

	inline IteratorPair GetIterators() noexcept { return Actions.second.GetIterators(); }
	inline ConstIteratorPair GetIterators() const noexcept { return Actions.second.GetIterators(); }

private:
	virtual void BindInputActions(FInputActionBindings& Actions) = 0;

private:
	std::pair<std::once_flag, FInputActionBindings> Actions;
};

class AInputController final
{
public:
	AInputController() : Id{} {}
	AInputController(AUniqueId&& UniqueId) : Id{ std::move(UniqueId) } {}

public:
	inline bool IsValid() const noexcept { return Id.IsValid(); }
	inline std::size_t GetHash() const noexcept { return Id.GetHash(); }
	inline void Release() noexcept { return Id.Release(); }

private:
	AUniqueId Id{};
};
