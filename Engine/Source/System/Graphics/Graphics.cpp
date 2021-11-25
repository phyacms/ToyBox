// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Graphics.h"
#include "IGraphicsContext.h"

FGraphics::FGraphics(FSystem& System)
	: System{ &System }
{
}

FGraphics::~FGraphics() noexcept
{
}

std::unique_ptr<IGraphicsContext> FGraphics::CreateContext(FSystemWindow& OutputWindow) &
{
	if (Renderer != nullptr)
	{
		auto Context{ Renderer->CreateContext(OutputWindow) };
		if (Context != nullptr && Context->IsValid())
		{
			return Context;
		}
	}

	return nullptr;
}
