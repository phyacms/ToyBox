// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "ByteBuffer.h"

class FByteStreamOutput final
{
public:
	explicit FByteStreamOutput(FByteBuffer& Buffer, std::endian Endianness = std::endian::little);
	FByteStreamOutput(FByteStreamOutput&&) noexcept = default;
	FByteStreamOutput& operator=(FByteStreamOutput&&) & noexcept = default;
	~FByteStreamOutput() noexcept { Release(); }

	FByteStreamOutput(const FByteStreamOutput&) = delete;
	FByteStreamOutput& operator=(const FByteStreamOutput&) & noexcept = delete;

	inline operator bool() const noexcept { return IsValid(); }
	template<
		typename T,
		typename = std::enable_if_t<TypeTraits::bIsTriviallySerializable<T>>>
	inline FByteStreamOutput& operator<<(T&& Value)
	{
		if (IsValid())
		{
			if (Endianness == std::endian::little) { Dest->Append<T, std::endian::little>(std::forward<T>(Value)); }
			else if (Endianness == std::endian::big) { Dest->Append<T, std::endian::big>(std::forward<T>(Value)); }
		}
		return *this;
	}
	template<
		typename T,
		typename = std::enable_if_t<std::is_base_of_v<ISerializable, T>>>
	inline FByteStreamOutput& operator<<(const T& Serializable)
	{
		if (IsValid())
		{
			const auto Serialized{ Serializable.Serialize() };
			operator<<(static_cast<std::uint64_t>(Serialized.GetByteSize()));
			Dest->Append(Serialized.GetPtr(), Serialized.GetByteSize());
		}
		return *this;
	}
	inline FByteStreamOutput& operator<<(std::endian Endianness) { this->Endianness = Endianness; }

public:
	inline bool IsValid() const noexcept { return Dest.IsValid(); }

private:
	bool TryAquire(FByteBuffer& Buffer) noexcept;
	void Release() noexcept;

private:
	AObject<FByteBuffer> Dest;
	std::endian Endianness;
};

class FByteStreamInput final
{
public:
	explicit FByteStreamInput(const FByteBuffer& Buffer, std::endian Endianness = std::endian::little);
	FByteStreamInput(const FByteStreamInput&) = default;
	FByteStreamInput& operator=(const FByteStreamInput &) & = default;
	FByteStreamInput(FByteStreamInput&&) noexcept = default;
	FByteStreamInput& operator=(FByteStreamInput&&) & noexcept = default;
	~FByteStreamInput() noexcept { Release(); }

	inline operator bool() const noexcept { return IsValid(); }
	template<
		typename T,
		typename = std::enable_if_t<TypeTraits::bIsSerializable<T>>>
	inline FByteStreamInput& operator>>(T& Value)
	{
		if (IsValid())
		{
			if constexpr (TypeTraits::bIsTriviallySerializable<T>)
			{
				if (Endianness == std::endian::little) { Value = Src->ReadAs<T, std::endian::little>(BytePos); BytePos += sizeof(T); }
				else if (Endianness == std::endian::big) { Value = Src->ReadAs<T, std::endian::big>(BytePos); BytePos += sizeof(T); }
			}
			else if constexpr (std::is_base_of_v<ISerializable, T>)
			{
				std::uint64_t ByteSize64{};
				operator>>(ByteSize64);
				const auto ByteSize{ static_cast<std::size_t>(ByteSize64) };
				FByteBuffer Bytes(ByteSize);
				std::memcpy(Bytes.GetPtr(), Src->GetPtr() + BytePos, ByteSize);
				Value.Deserialize(Bytes);
				BytePos += ByteSize;
			}
			else { static_assert(false); }
		}
		return *this;
	}
	inline FByteStreamInput& operator>>(std::endian Endianness) { this->Endianness = Endianness; }

public:
	inline bool IsValid() const noexcept { return Src.IsValid(); }

private:
	bool TryAquire(const FByteBuffer& Buffer) noexcept;
	void Release() noexcept;

private:
	AConstObject<FByteBuffer> Src;
	std::size_t BytePos;
	std::endian Endianness;
};
