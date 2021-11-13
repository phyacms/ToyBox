// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "String.h"
#include "Platform/Platform.h"

bool FString::Deserialize(const FByteBuffer& Bytes)
{
	std::vector<char8_t> Chars(Bytes.GetByteSize());
	std::transform(
		std::execution::par_unseq,
		Bytes.GetPtr(),
		Bytes.GetPtr() + Bytes.GetByteSize(),
		std::begin(Chars),
		[](Byte Value)->char8_t { return static_cast<char8_t>(Value); });

	auto Converted{ PlatformFunctions::UTF8ToString(Chars) };
	if (Converted.has_value())
	{
		*this = std::move(Converted).value();
		return true;
	}
	else
	{
		return false;
	}
}

FByteBuffer FString::Serialize() const
{
	FByteBuffer Bytes{};
	const auto& Converted{ PlatformFunctions::StringToUTF8(*this) };
	if (Converted.has_value())
	{
		const auto& UTF8Chars{ Converted.value() };
		Bytes.Reallocate(UTF8Chars.size());
		std::transform(
			std::execution::par_unseq,
			std::cbegin(UTF8Chars),
			std::cend(UTF8Chars),
			Bytes.GetPtr(),
			[](char8_t Ch)->Byte { return static_cast<Byte>(Ch); });
	}
	return Bytes;
}
