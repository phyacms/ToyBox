// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Coord.h"
#include "ScreenArea.h"

struct UDim final
{
	static const UDim Default;

	UCoord Offset{};
	UCoord Scale{};
};

inline FScreenArea ToScreenArea(const UDim& Dim, const FScreenArea& Base) noexcept
{
	const auto& Offset{ Dim.Offset.GetAsAbsolute(Base.Size).GetAsVector() };
	const auto& Diagonal{ Dim.Scale.GetAsAbsolute(Base.Size).GetAsVector() };
	return {
		.Location{ FScreenLocation{
			Base.Location.X() + Offset.X(),
			Base.Location.Y() + Offset.Y() } },
		.Size{ FScreenSize{
			Diagonal.X(),
			Diagonal.Y() } } };
}

class FDim final
{
public:
	explicit FDim(UDim Dimension = UDim::Default)
		: Dimension{ std::move(Dimension) }
		, CachedArea{} {}
	FDim(const FDim&) = default;
	FDim& operator=(const FDim&) & = default;
	FDim(FDim&&) noexcept = default;
	FDim& operator=(FDim&&) & noexcept = default;
	~FDim() noexcept = default;

public:
	inline void SetDimension(UDim Dimension, const FScreenArea& Base) { this->Dimension = std::move(Dimension); UpdateArea(Base); }
	inline void UpdateArea(const FScreenArea& Base) const noexcept { CachedArea = ::ToScreenArea(Dimension, Base); }
	inline const FScreenArea& GetArea() const noexcept { return CachedArea; }
	inline const FScreenArea& GetArea(const FScreenArea& Base) const noexcept { UpdateArea(Base); return GetArea(); }

private:
	UDim Dimension;
	mutable FScreenArea CachedArea;
};

class FAspectRatio final
{
public:
	FAspectRatio(UDim Dimension = UDim::Default, float AspectRatio = 0.0f)
		: FAspectRatio(std::move(Dimension), AspectRatio, AspectRatio) {}
	FAspectRatio(UDim Dimension, float MinimumAspectRatio, float MaximumAspectRatio)
		: Dimension{ std::move(Dimension) }
		, MinimumAspectRatio{ MinimumAspectRatio }
		, MaximumAspectRatio{ MaximumAspectRatio }
		, CachedArea{} {}

public:
	void SetDimension(UDim Dimension, const FScreenArea& Base) noexcept { this->Dimension.SetDimension(Dimension, Base); UpdateArea(); }
	void SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept;
	inline void SetAspectRatio(float AspectRatio) noexcept { SetAspectRatio(AspectRatio, AspectRatio); UpdateArea(); }
	inline void UpdateArea(const FScreenArea& Base) const noexcept { Dimension.UpdateArea(Base); UpdateArea(); }

	inline const FScreenArea& GetArea() const noexcept { return CachedArea; }
	inline const FScreenArea& GetArea(const FScreenArea& Base) const noexcept { UpdateArea(Base); return GetArea(); }

private:
	inline void UpdateArea() const noexcept { CachedArea = Dimension.GetArea().GetInnerAreaOf(MinimumAspectRatio, MaximumAspectRatio); }

private:
	FDim Dimension;
	float MinimumAspectRatio;
	float MaximumAspectRatio;
	mutable FScreenArea CachedArea;
};
