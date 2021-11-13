// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "ByteBuffer.h"

FByteBuffer::FByteBuffer(std::size_t ByteSize)
	: TObject<FByteBuffer>(*this)
	, Buffer{}
	, CurrentSize{}
	, IOContext{}
{
	Reallocate(ByteSize);
}

FByteBuffer::FByteBuffer(const FByteBuffer& Other)
	: TObject<FByteBuffer>(*this)
	, Buffer{ Other.Buffer }
	, CurrentSize{ Other.CurrentSize }
	, IOContext{}
{
}

FByteBuffer& FByteBuffer::operator=(const FByteBuffer& Other) &
{
	if (this != &Other)
	{
		Buffer = Other.Buffer;
		CurrentSize = Other.CurrentSize;
	}
	return *this;
}

FByteBuffer::FByteBuffer(FByteBuffer&& Other) noexcept
	: TObject<FByteBuffer>(std::move(Other))
	, Buffer{ std::move(Other.Buffer) }
	, CurrentSize{ std::move(Other.CurrentSize) }
	, IOContext{ std::move(Other.IOContext) }
{
	RedirectObjectPtr(*this);
	Other.CurrentSize = 0;
}

FByteBuffer& FByteBuffer::operator=(FByteBuffer&& Other) & noexcept
{
	if (this != &Other)
	{
		TObject<FByteBuffer>::operator=(std::move(Other));
		RedirectObjectPtr(*this);
		Buffer = std::move(Other.Buffer);
		CurrentSize = std::move(Other.CurrentSize);
		IOContext = std::move(Other.IOContext);
		Other.CurrentSize = 0;
	}
	return *this;
}

void FByteBuffer::Reserve(std::size_t ByteSize)
{
	if (Buffer.size() < ByteSize)
	{
		Buffer.resize(ByteSize);
	}
}

void FByteBuffer::Reallocate(std::size_t ByteSize)
{
	Reserve(ByteSize);
	CurrentSize = ByteSize;
}

FByteBuffer& FByteBuffer::Insert(std::size_t BytePos, const void* SrcData, std::size_t ByteSize)
{
	if (SrcData >= GetPtr()
		&& SrcData <= GetPtr() + ByteSize)
	{
		std::vector<Byte> Copied(ByteSize);
		std::memcpy(Copied.data(), SrcData, ByteSize);
		return Insert(BytePos, Copied.data(), ByteSize);
	}

	Reallocate(GetByteSize() + ByteSize);
	std::memcpy(GetPtr() + BytePos + ByteSize, GetPtr() + BytePos, ByteSize);
	std::memcpy(GetPtr() + BytePos, SrcData, ByteSize);
	return *this;
}

FByteBuffer& FByteBuffer::Append(const void* SrcData, std::size_t ByteSize)
{
	if (SrcData >= GetPtr()
		&& SrcData <= GetPtr() + ByteSize)
	{
		std::vector<Byte> Copied(ByteSize);
		std::memcpy(Copied.data(), SrcData, ByteSize);
		return Append(Copied.data(), ByteSize);
	}

	const auto BytePos{ GetByteSize() };
	Reallocate(GetByteSize() + ByteSize);
	std::memcpy(GetPtr() + BytePos, SrcData, ByteSize);
	return *this;
}

void FByteBuffer::Swap(FByteBuffer& Other) noexcept
{
	Buffer.swap(Other.Buffer);
	std::swap(CurrentSize, Other.CurrentSize);
	auto Tmp{ std::move(IOContext) };
	IOContext = std::move(Other.IOContext);
	Other.IOContext = std::move(Tmp);
}
