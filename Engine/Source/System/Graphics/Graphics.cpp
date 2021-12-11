// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Graphics.h"
#include "IGraphicsContext.h"

FGraphics::FGraphics(FSystem& System)
	: System{ &System }
	, Renderer{}
{
}

FGraphics::~FGraphics() noexcept
{
}

std::unique_ptr<IGraphicsContext> FGraphics::CreateContext(FSystemWindow& OutputWindow) &
{
	if (Renderer != nullptr)
	{
		if (auto Context{ Renderer->CreateContext(OutputWindow) };
			Context != nullptr && Context->IsValid())
		{
			return Context;
		}
	}
	return nullptr;
}

bool FGraphics::SetRenderer(std::unique_ptr<IGraphicsRenderer>&& Created) noexcept
{
	if (!Created->CreateStaticResources())
	{
		return false;
	}

	Renderer = std::move(Created);
	return true;
}
