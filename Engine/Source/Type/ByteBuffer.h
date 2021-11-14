// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Byte.h"
#include "Object.h"
#include "TypeTraits.h"

class FByteBuffer final
	: public TObject<FByteBuffer>
{
private:
	class FIOContext final
	{
	private:
		friend class FByteStreamOutput;
		friend class FByteStreamInput;
		inline static constexpr auto ExclusiveWrite{ static_cast<std::size_t>(-1) };

		std::size_t UseCount{};
	};

public:
	explicit FByteBuffer(std::size_t ByteSize = 0);
	FByteBuffer(const FByteBuffer& Other);
	FByteBuffer& operator=(const FByteBuffer& Other) &;
	FByteBuffer(FByteBuffer&& Other) noexcept;
	FByteBuffer& operator=(FByteBuffer&& Other) & noexcept;
	~FByteBuffer() noexcept = default;

	template<typename T>
	inline FByteBuffer& operator+=(T&& Param) { return Append(std::forward<T>(Param)); }

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

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FByteBuffer& Insert(std::size_t BytePos, const T& Value) { return Insert(BytePos, ::ToByteArray<T, Endianness>(Value).data(), sizeof(T)); }
	inline FByteBuffer& Insert(std::size_t BytePos, const FByteBuffer& Other) { return Insert(BytePos, Other.GetPtr(), Other.GetByteSize()); }
	FByteBuffer& Insert(std::size_t BytePos, const void* SrcData, std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FByteBuffer& Append(const T& Value) { return Append(::ToByteArray<T, Endianness>(Value).data(), sizeof(T)); }
	inline FByteBuffer& Append(const FByteBuffer& Other) { return Append(Other.GetPtr(), Other.GetByteSize()); }
	FByteBuffer& Append(const void* SrcData, std::size_t ByteSize);

	template<
		typename T,
		std::endian Endianness = std::endian::little,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline T ReadAs(std::size_t BytePos) const { T Value{}; std::memcpy(&Value, GetPtr() + BytePos, sizeof(T)); return Value; }
	inline FByteBuffer Read(std::size_t BytePos, std::size_t ByteSize) const { FByteBuffer Bytes(ByteSize); std::memcpy(Bytes.GetPtr(), GetPtr() + BytePos, ByteSize); return Bytes; }

	void Swap(FByteBuffer& Other) noexcept;

	inline Byte* GetPtr() noexcept { return Buffer.data(); }
	inline const Byte* GetPtr() const noexcept { return Buffer.data(); }
	inline FIOContext& GetIOContext() const noexcept { return IOContext; }

private:
	std::vector<Byte> Buffer;
	std::size_t CurrentSize;
	mutable FIOContext IOContext;
};

class ISerializable
{
public:
	virtual bool Deserialize(const FByteBuffer& Bytes) = 0;
	virtual FByteBuffer Serialize() const = 0;
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
