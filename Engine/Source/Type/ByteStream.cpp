// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Bytestream.h"

FByteStreamOutput::FByteStreamOutput(FByteBuffer& Buffer, std::endian Endianness)
	: Dest{}
	, Endianness{ Endianness }
{
	if (!TryAquire(Buffer))
	{
		Release();
	}
}

bool FByteStreamOutput::TryAquire(FByteBuffer& Buffer) noexcept
{
	bool bLocked{};
	auto& Ctx{ Buffer.GetIOContext() };
	if (Ctx.UseCount != 0)
	{
		return false;
	}
	Ctx.UseCount = Ctx.ExclusiveWrite;
	Dest = Buffer;
	return IsValid();
}

void FByteStreamOutput::Release() noexcept
{
	if (IsValid())
	{
		Dest->GetIOContext().UseCount = 0;
	}
}

FByteStreamInput::FByteStreamInput(const FByteBuffer& Buffer, std::endian Endianness)
	: Src{}
	, Endianness{ Endianness }
{
	if (!TryAquire(Buffer))
	{
		Release();
	}
}

bool FByteStreamInput::TryAquire(const FByteBuffer& Buffer) noexcept
{
	bool bLocked{};
	auto& Ctx{ Buffer.GetIOContext() };
	if (Ctx.UseCount == Ctx.ExclusiveWrite)
	{
		return false;
	}
	++Ctx.UseCount;
	Src = Buffer;
	return IsValid();
}

void FByteStreamInput::Release() noexcept
{
	if (IsValid())
	{
		--Src->GetIOContext().UseCount;
	}
}
