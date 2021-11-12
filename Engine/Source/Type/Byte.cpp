// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Byte.h"

FByteBuffer::FByteBuffer(std::size_t ByteSize)
	: TObject<FByteBuffer>(*this)
	, Buffer{}
	, CurrentSize{}
{
	Reallocate(ByteSize);
}

FByteBuffer::FByteBuffer(const FByteBuffer& Other)
	: TObject<FByteBuffer>(*this)
	, Buffer{ Other.Buffer }
	, CurrentSize{ Other.CurrentSize }
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
