// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IInputController.h"
#include "InputContext.h"

void AInputControllerBinding::Release() noexcept
{
	if (IsValid())
	{
		Context->UnbindInputController(*this);
	}
	Id.Release();
}
