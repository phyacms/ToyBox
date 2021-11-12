// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Byte.h"

FByteBuffer::FByteBuffer(FByteBuffer&& Other) noexcept
	: Buffer{ std::move(Other.Buffer) }
	, CurrentSize{ std::move(Other.CurrentSize) }
{
	Other.CurrentSize = 0;
}

FByteBuffer& FByteBuffer::operator=(FByteBuffer&& Other) & noexcept
{
	if (this != &Other)
	{
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

void FByteBuffer::ShrinkToFit()
{
	Buffer.resize(CurrentSize);
	Buffer.shrink_to_fit();
}
