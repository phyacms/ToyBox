// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Color.h"

FColor::UNorms::VectorType FColor::ConvertByteOrder(
	UNorms::VectorType Norms,
	EColorByteOrder From,
	EColorByteOrder To)
{
	if (From != To)
	{
		static constexpr auto RotateLeft{
			[](auto& Container)->void {
				std::rotate(
					std::begin(Container),
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
	const auto& Bytes{ ::ToByteArray<decltype(FColorCode::Code), std::endian::big>(ColorCode.Code) };
	Norms.Set<Byte>(Bytes, 0, 255);
	auto Vector{ Norms.GetAsVector() };


	Norms.Set<float>(ConvertByteOrder(Vector, ColorCode.ByteOrder, DefaultByteOrder));
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
		.Code{ ::FromByteArray<decltype(FColorCode::Code), std::endian::big>(Bytes.data()) },
		.ByteOrder{ ByteOrder } };
}

FColor::UNorms FColor::GetAsNormals(EColorByteOrder ByteOrder) const noexcept
{
	UNorms Norms{};
	Norms.Set<float>(ConvertByteOrder(this->Norms.GetAsVector(), DefaultByteOrder, ByteOrder));
	return Norms;
}

bool FColor::Deserialize(const FBytes& Bytes)
{
	Set(FColorCode{
		.Code{ Bytes.ReadAs<decltype(FColorCode::Code)>(0) },
		.ByteOrder{ EColorByteOrder::ARGB } });
	return true;
}

FBytes FColor::Serialize() const
{
	FBytes Bytes{};
	Bytes.Reserve(sizeof(FColorCode::Code));
	Bytes.Append(GetAsColorCode(EColorByteOrder::ARGB).Code);
	return Bytes;
}
