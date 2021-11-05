// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IGraphicsContext.h"
#include "System/Window/SystemWindow.h"

IGraphicsContext::IGraphicsContext(FSystemWindow& OutputWindow)
	: OutputWindow{ OutputWindow }
{
}
