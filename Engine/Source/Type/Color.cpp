// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Color.h"

FColor::UNorms FColor::ConvertByteOrder(UNorms Norms, EColorByteOrder From, EColorByteOrder To)
{
	if (From != To)
	{
		static constexpr auto RotateLeft{
			[](auto& Container)->void {
				std::rotate(std::begin(Container),
					std::begin(Container) + 1,
					std::end(Container)); } };

		static constexpr auto RotateRight{
			[](auto& Container)->void {
				std::rotate(
					std::rbegin(Container),
					std::rbegin(Container) + 1,
					std::rend(Container)); } };

		if (From == EColorByteOrder::ARGB && To == EColorByteOrder::RGBA)
		{
			RotateLeft(Norms);
		}
		else if (From == EColorByteOrder::RGBA && To == EColorByteOrder::ARGB)
		{
			RotateRight(Norms);
		}
	}

	return Norms;
}

FColor::FColor()
	: Norms{}
{
}

FColor::FColor(FColorCode ColorCode)
	: FColor()
{
	Set(std::move(ColorCode));
}

void FColor::Set(FColorCode ColorCode) noexcept
{
	const auto& Bytes{ ::ToByteArray<std::uint32_t, std::endian::big>(ColorCode.Code) };

	UNorms Norms{};
	std::transform(
		std::execution::par_unseq,
		std::cbegin(Bytes),
		std::cend(Bytes),
		std::begin(Norms),
		[](Byte Value)->float { return Value / 255.0f; });

	this->Norms = ConvertByteOrder(Norms, ColorCode.ByteOrder, DefaultByteOrder);
}

FColorCode FColor::GetAsColorCode(EColorByteOrder ByteOrder) const noexcept
{
	const auto& Norms{ GetAsNormals(ByteOrder) };

	std::array<Byte, 4> Bytes{};
	std::transform(
		std::execution::par_unseq,
		std::cbegin(Norms),
		std::cend(Norms),
		std::begin(Bytes),
		[](float Norm)->Byte { return static_cast<Byte>(Norm * 255.0f); });

	return {
		.Code{ ::FromByteArray<std::uint32_t, std::endian::big>(Bytes.data()) },
		.ByteOrder{ ByteOrder } };
}

FColor::UNorms FColor::GetAsNormals(EColorByteOrder ByteOrder) const noexcept
{
	return ConvertByteOrder(Norms, DefaultByteOrder, ByteOrder);
}

bool FColor::Deserialize(const FByteBuffer& Bytes)
{
	Set(FColorCode{
		.Code{ Bytes.ReadAs<std::uint32_t>(0) },
		.ByteOrder{ EColorByteOrder::ARGB } });
	return true;
}

FByteBuffer FColor::Serialize() const
{
	FByteBuffer Buffer{};
	Buffer.Reserve(sizeof(std::uint32_t));
	Buffer.Append(GetAsColorCode(EColorByteOrder::ARGB).Code);
	return Buffer;
}
