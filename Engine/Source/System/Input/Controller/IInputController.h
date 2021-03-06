// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Time.h"
#include "Type/Object/Object.h"
#include "Type/Resource/UniqueId.h"
#include "Type/ScreenSpace/ScreenLocation.h"
#include "System/Input/Chord/InputTrigger.h"

class FInputContext;

class IInputController
	: public TObject<IInputController>
{
public:
	IInputController() : TObject<IInputController>(*this) {}
	virtual ~IInputController() noexcept = default;

	IInputController(const IInputController&) = delete;
	IInputController& operator=(const IInputController&) & = delete;
	IInputController(IInputController&&) noexcept = delete;
	IInputController& operator=(IInputController&&) & noexcept = delete;

public:
	virtual bool DispatchInputAction(
		const FInputContext& Context,
		FTimePoint Time,
		const FInputTrigger& Trigger) const = 0;
	virtual bool DispatchMouseMovement(
		const FInputContext& Context,
		FTimePoint Time,
		const FScreenLocation::DiffType& Displacement) const { return false; };
};

class AInputControllerBinding final
{
	friend FInputContext;

public:
	AInputControllerBinding() : Context{}, Id{} {}

private:
	AInputControllerBinding(
		FInputContext& Context,
		AUniqueId&& UniqueId)
		: Id{ std::move(UniqueId) } {}

public:
	inline bool IsValid() const noexcept { return Context.IsValid() && Id.IsValid(); }
	inline std::size_t GetHash() const noexcept { return Context.IsValid() ? Id.GetHash() : FUniqueId::InvalidId; }
	void Release() noexcept;

private:
	TObjRef<FInputContext> Context;
	AUniqueId Id;
};
