// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Math/Normals.h"
#include "Byte.h"

enum class EColorByteOrder : std::size_t { ARGB, RGBA };

struct FColorCode final
{
	using ValueType = std::uint32_t;
	ValueType Code{};
	EColorByteOrder ByteOrder{};
};

namespace ColorCodes
{
	inline constexpr FColorCode White{ 0xFFFFFFFF, EColorByteOrder::ARGB };
	inline constexpr FColorCode Black{ 0xFF000000, EColorByteOrder::ARGB };
	inline constexpr FColorCode Red{ 0xFFFF0000, EColorByteOrder::ARGB };
	inline constexpr FColorCode Green{ 0xFF00FF00, EColorByteOrder::ARGB };
	inline constexpr FColorCode Blue{ 0xFF0000FF, EColorByteOrder::ARGB };
	inline constexpr FColorCode Cyan{ 0xFF00FFFF, EColorByteOrder::ARGB };
	inline constexpr FColorCode Magenta{ 0xFFFF00FF, EColorByteOrder::ARGB };
	inline constexpr FColorCode Yellow{ 0xFFFFFF00, EColorByteOrder::ARGB };
	inline constexpr FColorCode CornflowerBlue{ 0xFF6495ED, EColorByteOrder::ARGB };
}

// Represents 32-bit color.
class FColor final
	: public ISerializable
{
public:
	using UNorms = TNormals<float, 4>;
	inline static constexpr auto DefaultByteOrder{ EColorByteOrder::RGBA };

private:
	static UNorms::VectorType ConvertByteOrder(UNorms::VectorType Norms, EColorByteOrder From, EColorByteOrder To);

public:
	FColor();
	FColor(FColorCode ColorCode);
	FColor(const FColor&) = default;
	FColor& operator=(const FColor&) & = default;
	FColor(FColor&&) noexcept = default;
	FColor& operator=(FColor&&) & noexcept = default;
	~FColor() noexcept { static_assert(sizeof(UNorms) == sizeof(float) * 4); }

	friend bool operator==(const FColor&, const FColor&) noexcept = default;
	friend bool operator!=(const FColor&, const FColor&) noexcept = default;

public:
	void Set(FColorCode ColorCode) noexcept;
	inline void Set(FColor Color) noexcept { *this = std::move(Color); }

	FColorCode GetAsColorCode(EColorByteOrder ByteOrder) const noexcept;
	UNorms GetAsNormals(EColorByteOrder ByteOrder) const noexcept;

	// @NOTE: Returns pointer to RGBA normals.
	inline const float* GetPtr() const noexcept { return Norms.GetPtr(); }

	// @NOTE: Serialization/Deserialization is performed via conversion from/to ARGB color code.
	virtual bool Deserialize(const FBytes& Bytes) override final;
	virtual [[nodiscard]] FBytes Serialize() const override final;

private:
	UNorms Norms;
};
