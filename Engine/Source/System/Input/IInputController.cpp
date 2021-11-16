// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IInputController.h"
#include "InputContext.h"

IInputController::IInputController()
	: TObject<IInputController>(*this)
	, Actions{}
{
}

IInputController::~IInputController() noexcept
{
}

void IInputController::BindStatics()
{
	std::call_once(
		Actions.first,
		[this]()->void { BindInputActions(Actions.second); });
}

IInputController::IteratorPair IInputController::GetIterators() noexcept
{
	return !Actions.second.IsEmpty()
		? IteratorPair{ std::begin(Actions.second), std::end(Actions.second) }
		: IteratorPair{};
}

IInputController::ConstIteratorPair IInputController::GetIterators() const noexcept
{
	return !Actions.second.IsEmpty()
		? ConstIteratorPair{ std::cbegin(Actions.second), std::cend(Actions.second) }
		: ConstIteratorPair{};
}
