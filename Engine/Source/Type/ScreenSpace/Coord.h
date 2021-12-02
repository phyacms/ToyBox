// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "AbsCoord.h"
#include "RelCoord.h"

class UCoord final
{
public:
	UCoord(UAbsCoord Abs = {}, URelCoord Rel = {}) : Abs{ std::move(Abs) }, Rel{ std::move(Rel) } {}
	UCoord(URelCoord Rel) : UCoord({}, std::move(Rel)) {}
	UCoord(const UCoord&) = default;
	UCoord& operator=(const UCoord&) & = default;
	UCoord(UCoord&&) noexcept = default;
	UCoord& operator=(UCoord&&) & noexcept = default;
	~UCoord() noexcept = default;

	inline UCoord operator+() const noexcept { return *this; }
	UCoord operator-() const noexcept;

	inline UCoord operator+(const UCoord& U) const noexcept { UCoord V{ *this }; V += U; return V; }
	inline UCoord operator-(const UCoord& U) const noexcept { UCoord V{ *this }; V -= U; return V; }

	UCoord& operator+=(const UCoord& U) noexcept;
	UCoord& operator-=(const UCoord& U) noexcept;

public:
	UAbsCoord GetAsAbsolute(const FScreenSize& Size) const noexcept;
	URelCoord GetAsRelative(const FScreenSize& Size) const noexcept;

private:
	UAbsCoord Abs{};
	URelCoord Rel{};
};

inline UCoord operator+(UAbsCoord Abs, const UCoord& Coord) noexcept { return Coord + Abs; }
inline UCoord operator+(URelCoord Rel, const UCoord& Coord) noexcept { return Coord + Rel; }
inline UCoord operator-(UAbsCoord Abs, const UCoord& Coord) noexcept { return UCoord{ Abs } - Coord; }
inline UCoord operator-(URelCoord Rel, const UCoord& Coord) noexcept { return UCoord{ Rel } - Coord; }

inline UAbsCoord ToAbsolute(const UCoord& Coord, const FScreenSize& Size) noexcept { return Coord.GetAsAbsolute(Size); }
inline URelCoord ToRelative(const UCoord& Coord, const FScreenSize& Size) noexcept { return Coord.GetAsRelative(Size); }
