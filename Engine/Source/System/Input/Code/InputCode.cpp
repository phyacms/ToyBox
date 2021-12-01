// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "InputCode.h"

bool InputFunctions::IsValidInputCode(const FInputCode& InputCode) noexcept
{
	return std::visit(stdhelp::overloaded{
		[](EKeyboardKey Key)->bool { return InputFunctions::IsValidKey(Key); },
		[](EMouseButton Button)->bool { return InputFunctions::IsValidButton(Button); } },
		InputCode);
}
