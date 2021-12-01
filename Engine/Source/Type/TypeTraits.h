// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

namespace TypeTraits
{
	// Checks T matches one of given types Ts.
	template<typename T, typename... Ts>
	struct IsOf final
	{
		inline static constexpr auto Value{
			std::disjunction_v<std::is_same<T, Ts>...> };
	};
	template<typename T, typename... Ts>
	inline constexpr auto bIsOf{ IsOf<T, Ts...>::Value };

	// Checks whether every type of given Ts is distinct from any others.
	// https://stackoverflow.com/questions/18986560
	template<typename... Ts>
	struct IsUnique;
	template<>
	struct IsUnique<> final { inline static constexpr auto Value{ true }; };
	template<typename T, typename... Ts>
	struct IsUnique<T, Ts...> final
	{
		inline static constexpr auto Value{
			IsUnique<Ts...>::Value
			&& !bIsOf<T, Ts...> };
	};
	template<typename... Ts>
	inline constexpr auto bIsUnique{ IsUnique<Ts...>::Value };

	// Retrieves the index of a uniquely-existing type in the given tuple of Ts.
	// If type T is NOT an uniquely-existing type, the retrieved value is equal to std::size_t(-1).
	// c.f.) std::get<T>()
	namespace Tuple
	{
		struct InvalidIndex final { inline static constexpr auto Value{ static_cast<std::size_t>(-1) }; };
		inline constexpr auto InvalidIndexValue{ InvalidIndex::Value };
	}

	template<typename T, typename Tuple>
	struct IndexOf;
	template<typename T>
	struct IndexOf<T, std::tuple<>> final
	{
		inline static constexpr auto Value{ Tuple::InvalidIndexValue };
	};
	template<typename T, typename... Ts>
	struct IndexOf<T, std::tuple<T, Ts...>> final
	{
		inline static constexpr auto Value{
			IndexOf<T, std::tuple<Ts...>>::Value == Tuple::InvalidIndexValue
				? 0
				: Tuple::InvalidIndexValue };
	};
	template<typename T, typename T1, typename... Ts>
	struct IndexOf<T, std::tuple<T1, Ts...>> final
	{
		inline static constexpr auto Value{
			IndexOf<T, std::tuple<Ts...>>::Value != Tuple::InvalidIndexValue
				? (1 + IndexOf<T, std::tuple<Ts...>>::Value)
				: Tuple::InvalidIndexValue };
	};
	template<typename T, typename... Ts>
	inline constexpr auto IndexValueOf{ IndexOf<T, Ts...>::Value };
}
