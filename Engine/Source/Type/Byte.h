// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "TypeTraits.h"

using Byte = std::uint8_t;

class ISerializable;

namespace TypeTraits
{
	inline constexpr auto bIsLittleEndianness{ std::endian::native == std::endian::little };
	inline constexpr auto bIsBigEndianness{ std::endian::native == std::endian::big };
	inline constexpr auto bIsMixedEndianness{ !bIsLittleEndianness && !bIsBigEndianness };

	template<typename T>
	struct IsTriviallySerializable : public std::conditional_t<
		bIsAnyTypeOf<
		T,
		char8_t, char16_t, char32_t,
		std::int8_t, std::int16_t, std::int32_t, std::int64_t,
		std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>,
		std::true_type,
		std::false_type> {};

	template<typename T>
	inline constexpr auto bIsTriviallySerializable{ IsTriviallySerializable<T>::value };

	template<typename T>
	struct IsSerializable final : public std::conditional_t<
		bIsTriviallySerializable<T> || std::is_base_of_v<ISerializable, T>,
		std::true_type,
		std::false_type> {};

	template<typename T>
	inline constexpr auto bIsSerializable{ IsSerializable<T>::value };
}

namespace ByteOperation::Conversion
{
	template<
		typename T,
		std::endian Endiannessness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline std::array<Byte, sizeof(T)> ToBytes(const T& Value)
	{
		static_assert(!TypeTraits::bIsMixedEndianness);

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
		inline T FromBytes(const Byte* Bytes)
	{
		static_assert(!TypeTraits::bIsMixedEndianness);

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
}

class FByteBuffer final
{
public:
	FByteBuffer() : Buffer{}, CurrentSize{} {}
	FByteBuffer(const FByteBuffer&) = default;
	FByteBuffer& operator=(const FByteBuffer&) & = default;
	FByteBuffer(FByteBuffer&& Other) noexcept;
	FByteBuffer& operator=(FByteBuffer&& Other) & noexcept;
	~FByteBuffer() noexcept = default;

	inline Byte& operator[](std::size_t BytePos) { return Buffer[BytePos]; }
	inline const Byte& operator[](std::size_t BytePos) const { return Buffer[BytePos]; }

public:
	inline Byte& At(std::size_t BytePos) { return Buffer.at(BytePos); }
	inline const Byte& At(std::size_t BytePos) const { return Buffer.at(BytePos); }

	inline bool IsEmpty() const noexcept { return CurrentSize == 0; }
	inline std::size_t GetByteSize() const noexcept { return CurrentSize; }

	inline void Clear() noexcept { CurrentSize = 0; }
	void Reserve(std::size_t Reserve);
	void Reallocate(std::size_t ByteSize);
	void ShrinkToFit();

	inline Byte* GetPtr() noexcept { return Buffer.data(); }
	inline const Byte* GetPtr() const noexcept { return Buffer.data(); }

private:
	std::vector<Byte> Buffer;
	std::size_t CurrentSize;
};

class ISerializable
{
public:
	virtual ISerializable& Deserialize(const FByteBuffer& ByteBuffer) = 0;
	virtual FByteBuffer Serialize() const = 0;
};
