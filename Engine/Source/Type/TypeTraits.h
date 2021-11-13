// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

namespace TypeTraits
{
	template<typename T, typename... Ts>
	struct IsAnyTypeOf : public std::conditional_t<
		std::disjunction_v<std::is_same<T, Ts>...>,
		std::true_type,
		std::false_type> {};
	template<typename T, typename... Ts>
	inline constexpr auto bIsAnyTypeOf{ IsAnyTypeOf<T, Ts...>::value };

	template<typename T, typename... Ts>
	struct IsAllTypeOf : public std::conditional_t<
		std::conjunction_v<std::is_same<T, Ts>...>,
		std::true_type,
		std::false_type> {};
	template<typename T, typename... Ts>
	inline constexpr auto bIsAllTypeOf{ IsAllTypeOf<T, Ts...>::value };

	inline constexpr auto bIsLittleEndian{ std::endian::native == std::endian::little };
	inline constexpr auto bIsBigEndian{ std::endian::native == std::endian::big };
	inline constexpr auto bIsMixedEndian{ !bIsLittleEndian && !bIsBigEndian };

	template<typename T>
	struct IsTriviallySerializable : public std::conditional_t<
		bIsAnyTypeOf<T,
			char8_t, char16_t, char32_t,
			std::int8_t, std::int16_t, std::int32_t, std::int64_t,
			std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>,
		std::true_type,
		std::false_type> {};

	template<typename T>
	inline constexpr auto bIsTriviallySerializable{ IsTriviallySerializable<T>::value };
}
