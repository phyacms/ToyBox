// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Byte.h"
#include "TypeTraits.h"

enum class EStringLength : std::size_t { CodePoint, CodeUnit };

namespace Unicode::Encoding::UTF16
{
	using CodeUnit = char16_t;
	using StringType = std::u16string;

	constexpr const std::size_t MSB{ TypeTraits::bIsLittleEndian ? 1 : (TypeTraits::bIsBigEndian ? 0 : -1) };
	constexpr const std::size_t LSB{ TypeTraits::bIsLittleEndian ? 0 : (TypeTraits::bIsBigEndian ? 1 : -1) };
	static_assert(MSB != LSB && MSB + LSB == 1);

	constexpr const Byte HighSurrogateBits{ 0b110110 };
	constexpr const Byte LowSurrogateBits{ 0b110111 };

	inline constexpr Byte GetHighByte(CodeUnit Unit) noexcept { return ((const Byte*)(&Unit))[MSB]; }
	inline constexpr Byte GetLowByte(CodeUnit Unit) noexcept { return ((const Byte*)(&Unit))[LSB]; }

	inline constexpr bool IsHighSurrogate(CodeUnit Unit) noexcept { return GetHighByte(Unit) >> 2 == HighSurrogateBits; }
	inline constexpr bool IsLowSurrogate(CodeUnit Unit) noexcept { return GetHighByte(Unit) >> 2 == LowSurrogateBits; }

	inline constexpr bool IsSurrogate(CodeUnit Unit) noexcept { return IsHighSurrogate(Unit) || IsLowSurrogate(Unit); }
	inline std::size_t CountSurrogates(const StringType& Str) noexcept {
		return std::count_if(std::execution::par_unseq, std::cbegin(Str), std::cend(Str), &IsSurrogate); }
}

// Stores UTF16-LE code units.
class FString final
	: public ISerializable
{
public:
	using CodeUnit = Unicode::Encoding::UTF16::CodeUnit;
	using StringType = Unicode::Encoding::UTF16::StringType;
	static constexpr auto InvalidPos{ StringType::npos };

public:
	FString() : Str{} {}
	template<
		typename... Ts,
		typename = std::enable_if_t<std::is_constructible_v<StringType, Ts...>>>
	FString(Ts&&... Params) : Str{ std::forward<Ts>(Params)... } {}
	FString(const FString&) = default;
	FString(FString&&) noexcept = default;
	FString& operator=(const FString&) & = default;
	FString& operator=(FString&&) & noexcept = default;
	virtual ~FString() noexcept = default;

	friend inline bool operator==(const FString& Lhs, const FString& Rhs) noexcept = default;
	friend inline bool operator!=(const FString& Lhs, const FString& Rhs) noexcept = default;

	inline FString operator+(const FString& Other) const { return Concatenate(Other); }
	inline FString operator+(FString&& Other) const { return Concatenate(std::move(Other)); }

	inline FString& operator+=(const FString& Other) { return Append(Other); }
	inline FString& operator+=(FString&& Other) { return Append(std::move(Other)); }

public:
	inline bool IsEmpty() const noexcept { return Str.empty(); }
	inline void Clear() noexcept { Str.clear(); }

	template<EStringLength StrLenType>
	inline std::size_t Length() const noexcept
	{
		auto Length{ Str.size() };
		if constexpr (StrLenType == EStringLength::CodePoint)
		{
			Length -= Unicode::Encoding::UTF16::CountSurrogates(Str);
		}
		return Length;
	}
	inline std::size_t GetCharacterCount() const noexcept { return Length<EStringLength::CodePoint>(); }
	inline std::size_t GetByteSize() const noexcept { return Length<EStringLength::CodeUnit>(); }

	inline FString Concatenate(const FString& Other) const { return FString{ *this }.Append(Other); }
	inline FString Concatenate(FString&& Other) const { return FString{ *this }.Append(std::move(Other)); }
	inline FString& Append(const FString& Other) { Str.append(Other.Str); return *this; }
	inline FString& Append(FString&& Other) { Str.append(std::move(Other.Str)); return *this; }

	inline const StringType& GetStr() const& noexcept { return Str; }
	inline StringType GetStr() const&& noexcept { return Str; }
	inline const CodeUnit* GetPtr() const& noexcept { return GetStr().c_str(); }

	// @NOTE: Serialization/Deserialization is performed via UTF-8 encoding/decoding.
	virtual bool Deserialize(const FBytes& Bytes) override final;
	virtual [[nodiscard]] FBytes Serialize() const override final;

private:
	StringType Str;
};

namespace TypeConversion
{
	inline constexpr std::size_t StringBufferSize{ 128 };

	enum class IntegerBase : std::size_t
	{
		Decimal = 10,
		Binary = 2,
		Octal = 8,
		Hexadecimal = 16,
	};
}

template<
	typename IntegralType,
	typename = std::enable_if<std::is_integral_v<IntegralType>>>
inline [[nodiscard]] FString ToString(
	IntegralType Value,
	TypeConversion::IntegerBase Base = TypeConversion::IntegerBase::Decimal)
{
	std::array<char, TypeConversion::StringBufferSize> Buffer{};

	auto [EndPtr, Ec]
		= std::to_chars(
			Buffer.data(),
			Buffer.data() + Buffer.size(),
			Value,
			static_cast<int>(Base));
	if (Ec != std::errc{})
	{
		return {};
	}

	const auto ByteSize{ EndPtr - Buffer.data() };
	FString::StringType Converted(ByteSize, FString::CodeUnit{});
	std::transform(
		std::execution::par_unseq,
		Buffer.data(),
		EndPtr,
		std::begin(Converted),
		[](char Code)->FString::CodeUnit { return static_cast<FString::CodeUnit>(Code); });

	FString::StringType Prefix{};
	switch (Base)
	{
		using TypeConversion::IntegerBase;

		default:
		case IntegerBase::Decimal:
			break;

		case IntegerBase::Binary: Prefix += USTR("0b"); break;
		case IntegerBase::Octal: Prefix += USTR("0"); break;
		case IntegerBase::Hexadecimal: Prefix += USTR("0x"); break;
	}

	// @NOTE: Character code '-' is in universal ASCII code page.
	Converted.insert(Converted.find_first_of(u'-') + 1, Prefix);
	return std::move(Converted);
}

template<
	typename FloatingPointType,
	typename = std::enable_if_t<std::is_floating_point_v<FloatingPointType>>>
inline [[nodiscard]] FString ToString(
	FloatingPointType Value,
	std::chars_format Format = std::chars_format::general)
{
	std::array<char, TypeConversion::StringBufferSize> Buffer{};

	auto [EndPtr, Ec]
		= std::to_chars(
			Buffer.data(),
			Buffer.data() + Buffer.size(),
			Value,
			Format);
	if (Ec != std::errc{})
	{
		return {};
	}

	const auto ByteSize{ EndPtr - Buffer.data() };
	FString::StringType Converted(ByteSize, FString::CodeUnit{});
	std::transform(
		std::execution::par_unseq,
		Buffer.data(),
		EndPtr,
		std::begin(Converted),
		[](char Code)->FString::CodeUnit { return static_cast<FString::CodeUnit>(Code); });

	return std::move(Converted);
}

// Provided best matches for floading-point types.
inline [[nodiscard]] FString ToString(float Value, std::chars_format Format = std::chars_format::general) { return ::ToString<float>(Value, Format); }
inline [[nodiscard]] FString ToString(double Value, std::chars_format Format = std::chars_format::general) { return ::ToString<double>(Value, Format); }
inline [[nodiscard]] FString ToString(long double Value, std::chars_format Format = std::chars_format::general) { return ::ToString<long double>(Value, Format); }

template<typename T, typename... Ts>
inline [[nodiscard]] FString ToString(const std::atomic<T>& Value, Ts&&... Params)
{
	return ::ToString(Value.load(), std::forward<Ts>(Params)...);
}
