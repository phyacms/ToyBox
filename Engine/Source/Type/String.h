// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Byte.h"
#include "TypeTraits.h"

namespace Unicode::Encoding::UTF16
{
	using CodeUnit = char16_t;

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
	inline std::size_t CountSurrogates(const std::u16string& Str) noexcept {
		return std::count_if(std::execution::par_unseq, std::cbegin(Str), std::cend(Str), &IsSurrogate); }
}

// Stores UTF16-LE code units.
class FString final
{
public:
	using CodeUnit = Unicode::Encoding::UTF16::CodeUnit;
	static constexpr auto InvalidPos{ std::u16string::npos };
	enum class ELengthType { CodePoint, CodeUnit };

public:
	FString() : Data{} {}
	FString(const FString&) = default;
	FString(FString&&) noexcept = default;
	FString& operator=(const FString&) & = default;
	FString& operator=(FString&&) &noexcept = default;

	// @TODO: Define constrained constructors and assignment operators
	//        instead of using universal-referenced ones.
	template<typename... Ts>
	FString(Ts&&... Params) : Data{ std::forward<Ts>(Params)... } {}
	template<typename... Ts>
	FString& operator=(Ts&&... Params) { Data.operator=(std::forward<Ts>(Params)...); return *this; }

	virtual ~FString() noexcept = default;

	inline friend bool operator==(const FString& Lhs, const FString& Rhs) noexcept = default;
	inline friend bool operator!=(const FString& Lhs, const FString& Rhs) noexcept = default;

	template<typename T>
	inline FString& operator+=(T&& Param) { Data += std::forward<T>(Param); return *this; }
	inline FString& operator+=(const FString& Other) { return operator+=(Other.Data); }

public:
	inline bool IsEmpty() const noexcept { return Data.empty(); }
	inline void Clear() noexcept { Data.clear(); }

	template<ELengthType LengthType>
	inline std::size_t Length() const noexcept
	{
		auto Length{ Data.size() };
		if constexpr (LengthType == ELengthType::CodePoint) { Length -= Unicode::Encoding::UTF16::CountSurrogates(Data); }
		return Length;
	}

	template<typename... Ts>
	inline FString& Insert(Ts&&... Params) { Data.insert(std::forward<Ts>(Params)...); return *this; }
	inline FString& Insert(std::size_t Index, const FString& Other) { return Insert(Index, Other.Data); }

	inline std::size_t FindFirstUnitOf(CodeUnit Unit, std::size_t UnitPos = 0) const noexcept { return Data.find_first_of(Unit, UnitPos); }
	inline std::size_t FindFirstUnitNotOf(CodeUnit Unit, std::size_t UnitPos = 0) const noexcept { return Data.find_first_not_of(Unit, UnitPos); }
	inline std::size_t FindLastUnitOf(CodeUnit Unit, std::size_t UnitPos = 0) const noexcept { return Data.find_last_of(Unit, UnitPos); }
	inline std::size_t FindLastUnitNotOf(CodeUnit Unit, std::size_t UnitPos = 0) const noexcept { return Data.find_last_not_of(Unit, UnitPos); }

	inline const CodeUnit* GetStr() const noexcept { return Data.c_str(); }

private:
	std::u16string Data;
};

using FStringView = const FString&;

namespace TypeConversion
{
	enum class IntegerBase : int
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
FString ToString(
	IntegralType Value,
	TypeConversion::IntegerBase Base = TypeConversion::IntegerBase::Decimal)
{
	std::array<char, 128> Buffer{};
	auto [EndPtr, Ec]
		= std::to_chars(
			Buffer.data(),
			Buffer.data() + Buffer.size(),
			Value,
			static_cast<int>(Base));

	FString Converted{};
	if (Ec == decltype(Ec){})
	{
		FString Prefix{};
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

		std::transform(
			Buffer.data(),
			EndPtr,
			std::back_inserter(Converted),
			[](char Code)->FString::CodeUnit { return static_cast<FString::CodeUnit>(Code); });

		Converted.Insert(Converted.FindFirstUnitOf(u'-') + 1, Prefix);
	}
	return Converted;
}

template<typename T, typename... Ts>
inline FString ToString(const std::atomic<T>& Value, Ts&&... Params)
{
	return ::ToString(Value.load(), std::forward<Ts>(Params)...);
}
