// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Color.h"
#include "System/Window/ScreenSpace.h"

struct FStrokeStyle final
{
	FColor Color{ ColorCodes::Black };
	float Width{ 1.0f };
};

class ISurface2D
{
public:
	ISurface2D() = default;
	virtual ~ISurface2D() noexcept = default;

	ISurface2D(const ISurface2D&) = delete;
	ISurface2D& operator=(const ISurface2D&) & = delete;
	ISurface2D(ISurface2D&&) noexcept = delete;
	ISurface2D& operator=(ISurface2D&&) & noexcept = delete;

public:
	virtual void DrawLine(const FScreenLocation& Begin, const FScreenLocation& End, const FStrokeStyle& Stroke = {}) = 0;
	virtual void DrawRect(const FScreenArea& Rect, const FStrokeStyle& Stroke = {}) = 0;
};
