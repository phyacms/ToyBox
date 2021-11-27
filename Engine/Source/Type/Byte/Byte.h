// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "TypeTraits.h"

using Byte = std::uint8_t;

template<
	typename T,
	std::endian Endiannessness = std::endian::little,
	typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
inline [[nodiscard]] std::array<Byte, sizeof(T)> ToByteArray(const T& Value)
{
	static_assert(!TypeTraits::bIsMixedEndian);

	std::array<Byte, sizeof(T)> Bytes{};
	std::memcpy(Bytes.data(), &Value, sizeof(T));
	if constexpr (sizeof(T) != 1 && Endiannessness != std::endian::native)
	{
		std::reverse(
			std::execution::par_unseq,
			std::begin(Bytes),
			std::end(Bytes));
	}
	return Bytes;
}

template<
	typename T,
	std::endian Endiannessness = std::endian::little,
	typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
inline [[nodiscard]] T FromByteArray(const Byte* Bytes)
{
	static_assert(!TypeTraits::bIsMixedEndian);

	T Value{};
	std::memcpy(&Value, Bytes, sizeof(T));
	if constexpr (sizeof(T) != 1 && Endiannessness != std::endian::native)
	{
		std::reverse(
			std::execution::par_unseq,
			reinterpret_cast<Byte*>(&Value),
			reinterpret_cast<Byte*>(&Value) + sizeof(T));
	}
	return Value;
}
