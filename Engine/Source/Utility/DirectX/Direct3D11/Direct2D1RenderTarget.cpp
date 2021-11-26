// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct2D1RenderTarget.h"

#ifdef PLATFORM_WINDOWS

FDirect2D1RenderTarget::FDirect2D1RenderTarget()
	: RenderTarget{}
	, Brushes{}
{
}

bool FDirect2D1RenderTarget::Initialize(ID2D1Factory& Factory, IDXGISurface& Surface) noexcept
{
	const D2D1_RENDER_TARGET_PROPERTIES& Properties = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT::DXGI_FORMAT_UNKNOWN,
			D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED),
		96.0f,
		96.0f,
		D2D1_RENDER_TARGET_USAGE::D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL::D2D1_FEATURE_LEVEL_DEFAULT);

	if (FAILED(Factory.CreateDxgiSurfaceRenderTarget(
		&Surface,
		&Properties,
		&RenderTarget)))
	{
		return false;
	}

	return true;
}

void FDirect2D1RenderTarget::DrawLine(
	const FScreenLocation& Begin,
	const FScreenLocation& End,
	const FStrokeStyle& Stroke)
{
	RenderTarget->DrawLine(
		::ToPoint(Begin),
		::ToPoint(End),
		&GetBrush(Stroke.Color),
		Stroke.Width,
		nullptr);
}

void FDirect2D1RenderTarget::DrawRect(
	const FScreenArea& Rect,
	const FStrokeStyle& Stroke)
{
	RenderTarget->DrawRectangle(
		::ToRect(Rect),
		&GetBrush(Stroke.Color),
		Stroke.Width,
		nullptr);
}

ID2D1SolidColorBrush& FDirect2D1RenderTarget::GetBrush(const FColor& Color)
{
	const auto& Index{ Color.GetAsColorCode(EColorByteOrder::ARGB).Code };
	if (Brushes.find(Index) == std::cend(Brushes))
	{
		const auto& [cIt, bEmplaced] = Brushes.emplace(Index, decltype(Brushes)::mapped_type{});
		if (!bEmplaced)
		{
			throw std::runtime_error{ __FUNCTION__ "(): Failed to emplace new element in std::unordered_map." };
		}

		if (FAILED(RenderTarget->CreateSolidColorBrush(::ToColor(Color), &Brushes.at(Index))))
		{
			throw std::runtime_error{ __FUNCTION__ "(): Failed to create solid color brush." };
		}
	}
	return *Brushes.at(Index).Get();
}

#endif
