// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Math/Vector.h"
#include "Math/Normals.h"
#include "System/Window/ScreenSpace.h"

using UVector = TVector<float, 2>;

class UAbsCoord;
class URelCoord;

class UAbsCoord final
{
	friend UAbsCoord ToAbsolute(const URelCoord& Rel, const FScreenSize& Size) noexcept;
	friend class UCoord;

public:
	UAbsCoord(const FScreenLocation& Coord = {});
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == UVector::Dimension>>
		explicit UAbsCoord(Parameters&&... Params) : Coord{ static_cast<UVector::ValueType>(Params)... } {}
	UAbsCoord(const UAbsCoord&) = default;
	UAbsCoord& operator=(const UAbsCoord&) & = default;
	UAbsCoord(UAbsCoord&&) noexcept = default;
	UAbsCoord& operator=(UAbsCoord&&) & noexcept = default;
	~UAbsCoord() noexcept = default;

public:
	inline const UVector& GetAsVector() const noexcept { return Coord; }
	inline const float* GetPtr() const noexcept { return Coord.GetPtr(); }

private:
	UVector Coord;
};

class URelCoord final
{
	friend URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept;
	friend class UCoord;

public:
	using FNormals = TNormals<float, 2>;
	static_assert(std::is_same_v<FNormals::VectorType, UVector>);

public:
	URelCoord(FNormals Coord = {}) : Norms{ std::move(Coord) } {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == UVector::Dimension>>
		explicit URelCoord(Parameters&&... Params) : Norms{ static_cast<UVector::ValueType>(Params)... } {}
	URelCoord(const URelCoord&) = default;
	URelCoord& operator=(const URelCoord&) & = default;
	URelCoord(URelCoord&&) noexcept = default;
	URelCoord& operator=(URelCoord&&) & noexcept = default;
	~URelCoord() noexcept = default;

public:
	inline const UVector& GetAsVector() const noexcept { return Norms.GetAsVector(); }
	inline const float* GetPtr() const noexcept { return Norms.GetPtr(); }

private:
	FNormals Norms;
};

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

struct URect final
{
	UCoord TopLeft{};
	UCoord BottomRight{};
};
