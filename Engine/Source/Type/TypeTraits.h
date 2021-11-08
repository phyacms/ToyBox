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
}
