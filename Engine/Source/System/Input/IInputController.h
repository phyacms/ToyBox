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

public:
	IInputController() : TObject<IInputController>(*this), Actions{} {}
	virtual ~IInputController() noexcept = default;

	IInputController(const IInputController&) = delete;
	IInputController& operator=(const IInputController&) & = delete;
	IInputController(IInputController&&) noexcept = delete;
	IInputController& operator=(IInputController&&) & noexcept = delete;

public:
	EnumerateConstIterators(Actions.second, ExposeConstIterators)

public:
	inline void BindStatics() {
		std::call_once(
			Actions.first,
			[this]()->void { BindInputActions(Actions.second); }); }

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
	inline void Release() noexcept { Id.Release(); }

private:
	AUniqueId Id{};
};
