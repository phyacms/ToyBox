// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Math/Normals.h"
#include "AbsCoord.h"

class UAbsCoord;

class URelCoord final
{
	friend URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept;
	friend class UCoord;

public:
	using VectorType = TVector<float, 2>;

private:
	using FNormals = TNormals<float, 2>;
	static_assert(std::is_same_v<FNormals::VectorType, VectorType>);

public:
	URelCoord(FNormals Coord = {}) : Norms{ std::move(Coord) } {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == VectorType::Dimension>>
		explicit URelCoord(Parameters&&... Params) : Norms{ static_cast<VectorType::ValueType>(Params)... } {}
	URelCoord(const URelCoord&) = default;
	URelCoord& operator=(const URelCoord&) & = default;
	URelCoord(URelCoord&&) noexcept = default;
	URelCoord& operator=(URelCoord&&) & noexcept = default;
	~URelCoord() noexcept = default;

public:
	inline const VectorType& GetAsVector() const noexcept { return Norms.GetAsVector(); }
	inline const float* GetPtr() const noexcept { return Norms.GetPtr(); }

private:
	FNormals Norms;
};

URelCoord ToRelative(const UAbsCoord& Abs, const FScreenSize& Size) noexcept;
