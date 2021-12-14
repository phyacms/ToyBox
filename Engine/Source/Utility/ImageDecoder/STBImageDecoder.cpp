// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "STBImageDecoder.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

struct STBImage final
{
	stbi_uc* Bytes{};
	int Width{};
	int Height{};
	int Channels{};

	explicit STBImage(const fs::path& ImageFilePath)
		: Bytes{}
		, Width{}
		, Height{}
		, Channels{} { Bytes = ::stbi_load(ImageFilePath.generic_string().c_str(), &Width, &Height, &Channels, 4); }
	~STBImage() noexcept { if (IsValid()) { ::stbi_image_free(Bytes); } }

	inline bool IsValid() const noexcept { return Bytes != nullptr; }
};

FImageFrame FSTBImageDecoder::LoadImage(const fs::path& ImageFilePath) const noexcept
{
	if (!IsValid())
	{
		return {};
	}

	STBImage LoadedImage{ ImageFilePath };
	if (!LoadedImage.IsValid())
	{
		return {};
	}

	const auto RowPitch{ static_cast<std::size_t>(LoadedImage.Width) * 4 };
	FImageFrame Frame{
		.RGBAs{ FBytes{ RowPitch * LoadedImage.Height } },
		.RowPitch{ RowPitch } };
	std::memcpy(Frame.RGBAs.GetPtr(), LoadedImage.Bytes, Frame.RGBAs.GetByteSize());
	return Frame;
}
