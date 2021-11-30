// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Byte.h"

FBytes::FBytes(std::size_t ByteSize)
	: Buffer{}
	, CurrentSize{}
{
	Reallocate(ByteSize);
}

FBytes::FBytes(const FBytes& Other)
	: Buffer{ Other.Buffer }
	, CurrentSize{ Other.CurrentSize }
{
}

FBytes& FBytes::operator=(const FBytes& Other) &
{
	if (this != &Other)
	{
		Buffer = Other.Buffer;
		CurrentSize = Other.CurrentSize;
	}
	return *this;
}

FBytes::FBytes(FBytes&& Other) noexcept
	: Buffer{ std::move(Other.Buffer) }
	, CurrentSize{ std::move(Other.CurrentSize) }
{
	Other.CurrentSize = 0;
}

FBytes& FBytes::operator=(FBytes&& Other) & noexcept
{
	if (this != &Other)
	{
		Buffer = std::move(Other.Buffer);
		CurrentSize = std::move(Other.CurrentSize);
		Other.CurrentSize = 0;
	}
	return *this;
}

void FBytes::Reserve(std::size_t ByteSize)
{
	if (Buffer.size() < ByteSize)
	{
		Buffer.resize(ByteSize);
	}
}

void FBytes::Reallocate(std::size_t ByteSize)
{
	Reserve(ByteSize);
	CurrentSize = ByteSize;
}

FBytes& FBytes::Insert(std::size_t BytePos, const void* SrcData, std::size_t ByteSize)
{
	if (ByteSize != 0)
	{
		if (IsInRange(SrcData))
		{
			std::vector<Byte> Copied(ByteSize);
			std::memcpy(Copied.data(), SrcData, ByteSize);
			return Insert(BytePos, Copied.data(), ByteSize);
		}

		Reallocate(GetByteSize() + ByteSize);
		std::memcpy(GetPtr() + BytePos + ByteSize, GetPtr() + BytePos, ByteSize);
		std::memcpy(GetPtr() + BytePos, SrcData, ByteSize);
	}
	return *this;
}

FBytes& FBytes::Append(const void* SrcData, std::size_t ByteSize)
{
	if (ByteSize != 0)
	{
		if (IsInRange(SrcData))
		{
			std::vector<Byte> Copied(ByteSize);
			std::memcpy(Copied.data(), SrcData, ByteSize);
			return Append(Copied.data(), ByteSize);
		}

		const auto BytePos{ GetByteSize() };
		Reallocate(GetByteSize() + ByteSize);
		std::memcpy(GetPtr() + BytePos, SrcData, ByteSize);
	}
	return *this;
}

FBytes& FBytes::Overwrite(std::size_t BytePos, const void* SrcData, std::size_t ByteSize)
{
	std::memmove(GetPtr() + BytePos, SrcData, ByteSize);
	return *this;
}

void FBytes::Swap(FBytes& Other) noexcept
{
	Buffer.swap(Other.Buffer);
	std::swap(CurrentSize, Other.CurrentSize);
}
