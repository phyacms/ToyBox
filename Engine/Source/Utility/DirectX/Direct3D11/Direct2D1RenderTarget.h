// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"
#include "System/Graphics/ISurface2D.h"

class FSystemWindow;

class FDirect2D1RenderTarget final
	: public ISurface2D
{
public:
	FDirect2D1RenderTarget();
	virtual ~FDirect2D1RenderTarget() noexcept { Terminate(); }

	FDirect2D1RenderTarget(FDirect2D1RenderTarget&&) noexcept = default;
	FDirect2D1RenderTarget& operator=(FDirect2D1RenderTarget&&) & noexcept = default;

public:
	bool Initialize(ID2D1Factory& Factory, IDXGISurface& Surface) noexcept;
	inline void Terminate() noexcept { RenderTarget.Reset(); }

	inline bool IsValid() const noexcept { return RenderTarget != nullptr; }
	inline ID2D1RenderTarget& GetRenderTarget() const& noexcept { return *RenderTarget.Get(); }

	virtual void DrawLine(const FScreenLocation& Begin, const FScreenLocation& End, const FStrokeStyle& Stroke = {}) override final;
	virtual void DrawRect(const FScreenArea& Rect, const FStrokeStyle& Stroke = {}) override final;

private:
	ID2D1SolidColorBrush& GetBrush(const FColor& Color);

private:
	TComPtr<ID2D1RenderTarget> RenderTarget;
	std::unordered_map<FColorCode::ValueType, TComPtr<ID2D1SolidColorBrush>> Brushes;
};

inline D2D1_POINT_2F ToPoint(const FScreenLocation& Point) noexcept
{
	return D2D1::Point2F(
		Point.X<FLOAT>(),
		Point.Y<FLOAT>());
}

inline D2D1_RECT_F ToRect(const FScreenArea& Rect) noexcept
{
	return D2D1::RectF(
		Rect.Location.X<FLOAT>(),
		Rect.Location.Y<FLOAT>(),
		Rect.Location.X<FLOAT>() + Rect.Size.X<FLOAT>(),
		Rect.Location.Y<FLOAT>() + Rect.Size.Y<FLOAT>());
}

inline D2D1_COLOR_F ToColor(const FColor& Color) noexcept
{
	D2D1_COLOR_F D2DColor{};
	std::memcpy(&D2DColor, Color.GetPtr(), sizeof(D2D1_COLOR_F));
	return D2DColor;
}

#endif
