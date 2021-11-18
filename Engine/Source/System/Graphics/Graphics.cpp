// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Graphics.h"
#include "IGraphicsRenderer.h"
#ifdef PLATFORM_WINDOWS
#include "Direct3D11/Direct3D11Renderer.h"
#endif
#include "IGraphicsContext.h"

FGraphics::FGraphics(FSystem& System)
	: System{ &System }
{
}

FGraphics::~FGraphics() noexcept
{
}

bool FGraphics::SetRendererType(EGraphicsRendererType RendererType) noexcept
{
	switch (RendererType)
	{
		case EGraphicsRendererType::Direct3D11:
#ifdef PLATFORM_WINDOWS
		{
			Renderer = std::make_unique<FDirect3D11Renderer>(*this);
		}
		break;
#endif

		default:
		case EGraphicsRendererType::Unknown:
			break;
	}

	if (Renderer != nullptr && Renderer->IsValid())
	{
		return true;
	}
	else
	{
		Renderer.reset();
		return false;
	}
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
