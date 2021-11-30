// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "String.h"
#include "Platform/Platform.h"

bool FString::Deserialize(const FBytes& Bytes)
{
	std::vector<char8_t> Chars(Bytes.GetByteSize());
	std::transform(
		std::execution::par_unseq,
		Bytes.GetPtr(),
		Bytes.GetPtr() + Bytes.GetByteSize(),
		std::begin(Chars),
		[](Byte Value)->char8_t { return static_cast<char8_t>(Value); });

	if (auto Str{ PlatformFunctions::UTF8ToString(Chars) };
		Str.has_value())
	{
		*this = std::move(Str.value());
		return true;
	}
	else
	{
		return false;
	}
}

FBytes FString::Serialize() const
{
	FBytes Bytes{};
	if (const auto& UTF8Str{ PlatformFunctions::StringToUTF8(*this) };
		UTF8Str.has_value())
	{
		const auto& UTF8Chars{ UTF8Str.value() };
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
