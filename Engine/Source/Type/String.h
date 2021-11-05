// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

using FString = std::u16string; // Stores UTF16-LE code units.
using FStringView = std::u16string_view;

namespace String::Conversion
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
	String::Conversion::IntegerBase Base = String::Conversion::IntegerBase::Decimal)
{
	std::array<char, 128> Buffer{};
	auto [End, Ec]
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
			using String::Conversion::IntegerBase;

			default:
			case IntegerBase::Decimal:
				break;

			case IntegerBase::Binary: Prefix += USTR("0b"); break;
			case IntegerBase::Octal: Prefix += USTR("0"); break;
			case IntegerBase::Hexadecimal: Prefix += USTR("0x"); break;
		}

		std::transform(
			Buffer.data(),
			End,
			std::back_inserter(Converted),
			[](char Code)->char16_t { return static_cast<char16_t>(Code); });

		Converted.insert(Converted.find_first_of(u'-') + 1, Prefix);
	}
	return Converted;
}

template<typename T, typename... Ts>
inline FString ToString(const std::atomic<T>& Value, Ts&&... Params)
{
	return ToString(Value.load(), std::forward<Ts>(Params)...);
}
