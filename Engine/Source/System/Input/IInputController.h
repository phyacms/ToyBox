// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

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

	using IteratorPair = std::pair<Iterator, Iterator>;
	using ConstIteratorPair = std::pair<ConstIterator, ConstIterator>;

public:
	IInputController();
	virtual ~IInputController() noexcept;

	IInputController(const IInputController&) = delete;
	IInputController& operator=(const IInputController&) & = delete;
	IInputController(IInputController&&) noexcept = delete;
	IInputController& operator=(IInputController&&) & noexcept = delete;

public:
	void BindStatics();

private:
	virtual void BindInputActions(FInputActionBindings& Actions) = 0;

	IteratorPair GetIterators() noexcept;
	ConstIteratorPair GetIterators() const noexcept;

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
