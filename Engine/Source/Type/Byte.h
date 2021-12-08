// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Traits.h"

using Byte = std::uint8_t;

namespace TypeTraits
{
	inline constexpr auto bIsLittleEndian{ std::endian::native == std::endian::little };
	inline constexpr auto bIsBigEndian{ std::endian::native == std::endian::big };
	inline constexpr auto bIsMixedEndian{ !bIsLittleEndian && !bIsBigEndian };

	template<typename T>
	struct IsTriviallySerializable : public std::conditional_t<
		bIsOf<T,
		char8_t, char16_t, char32_t,
		std::int8_t, std::int16_t, std::int32_t, std::int64_t,
		std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>,
		std::true_type,
		std::false_type> {};

	template<typename T>
	inline constexpr auto bIsTriviallySerializable{ IsTriviallySerializable<T>::value };
}

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

class FBytes final
{
public:
	explicit FBytes(std::size_t ByteSize = 0);
	FBytes(const FBytes& Other);
	FBytes& operator=(const FBytes& Other) &;
	FBytes(FBytes&& Other) noexcept;
	FBytes& operator=(FBytes&& Other) & noexcept;
	~FBytes() noexcept = default;

	template<typename T>
	inline FBytes& operator+=(T&& Param) { return Append(std::forward<T>(Param)); }

public:
	inline Byte& operator[](std::size_t BytePos) & { return Buffer[BytePos]; }
	inline const Byte& operator[](std::size_t BytePos) const& { return Buffer[BytePos]; }
	inline Byte operator[](std::size_t BytePos) const&& { return Buffer[BytePos]; }

	inline Byte& At(std::size_t BytePos) & { return Buffer.at(BytePos); }
	inline const Byte& At(std::size_t BytePos) const& { return Buffer.at(BytePos); }
	inline Byte At(std::size_t BytePos) const&& { return Buffer.at(BytePos); }

public:
	inline bool IsEmpty() const noexcept { return CurrentSize == 0; }
	inline std::size_t GetByteSize() const noexcept { return CurrentSize; }

	inline void Clear() noexcept { CurrentSize = 0; }
	void Reserve(std::size_t Reserve);
	void Reallocate(std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FBytes& Insert(std::size_t BytePos, const T& Value) { return Insert(BytePos, ::ToByteArray<T, Endianness>(Value).data(), sizeof(T)); }
	inline FBytes& Insert(std::size_t BytePos, const FBytes& Other) { return Insert(BytePos, Other.GetPtr(), Other.GetByteSize()); }
	FBytes& Insert(std::size_t BytePos, const void* SrcData, std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FBytes& Append(const T& Value) { return Append(::ToByteArray<T, Endianness>(Value).data(), sizeof(T)); }
	inline FBytes& Append(const FBytes& Other) { return Append(Other.GetPtr(), Other.GetByteSize()); }
	FBytes& Append(const void* SrcData, std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FBytes& Overwrite(std::size_t BytePos, const T& Value) { return Overwrite(BytePos, ::ToByteArray<T, Endianness>(Value).data(), sizeof(T)); }
	inline FBytes& Overwrite(std::size_t BytePos, const FBytes& Other) { return Overwrite(BytePos, Other.GetPtr(), Other.GetByteSize()); }
	FBytes& Overwrite(std::size_t BytePos, const void* SrcData, std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline T ReadAs(std::size_t BytePos) const { T Value{}; std::memcpy(&Value, GetPtr() + BytePos, sizeof(T)); return Value; }
	inline FBytes Read(std::size_t BytePos, std::size_t ByteSize) const { FBytes Bytes(ByteSize); std::memcpy(Bytes.GetPtr(), GetPtr() + BytePos, ByteSize); return Bytes; }

	void Swap(FBytes& Other) noexcept;

	inline Byte* GetPtr() & noexcept { return Buffer.data(); }
	inline const Byte* GetPtr() const& noexcept { return Buffer.data(); }

private:
	inline bool IsInRange(const void* Ptr) const noexcept
	{
		return Ptr != nullptr
			? Ptr >= GetPtr() && Ptr < GetPtr() + GetByteSize()
			: false;
	}

private:
	std::vector<Byte> Buffer;
	std::size_t CurrentSize;
};

class ISerializable
{
public:
	virtual bool Deserialize(const FBytes& Bytes) = 0;
	virtual [[nodiscard]] FBytes Serialize() const = 0;
};

namespace TypeTraits
{
	template<typename T>
	struct IsSerializable final : public std::conditional_t<
		bIsTriviallySerializable<T> || std::is_base_of_v<ISerializable, T>,
		std::true_type,
		std::false_type> {};

	template<typename T>
	inline constexpr auto bIsSerializable{ IsSerializable<T>::value };
}
