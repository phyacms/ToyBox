// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Byte.h"

struct FImageFrame final
{
	FBytes RGBAs{};
	std::size_t RowPitch{};

	inline bool IsValid() const noexcept {
		return RowPitch != 0
			&& !RGBAs.IsEmpty()
			&& RGBAs.GetByteSize() % (RowPitch) == 0; }

	template<typename T = std::size_t>
	inline T Width() const noexcept { return static_cast<T>(RowPitch / 4); }
	template<typename T = std::size_t>
	inline T Height() const noexcept { return static_cast<T>(RGBAs.GetByteSize() / RowPitch); }
};

class IImageDecoder
{
public:
	IImageDecoder() = default;
	IImageDecoder(const IImageDecoder&) = default;
	IImageDecoder& operator=(const IImageDecoder&) & = default;
	IImageDecoder(IImageDecoder&&) noexcept = default;
	IImageDecoder& operator=(IImageDecoder&&) & noexcept = default;
	virtual ~IImageDecoder() noexcept = default;

public:
	virtual bool IsValid() const noexcept = 0;
	virtual FImageFrame LoadImage(const fs::path& ImageFilePath) const noexcept = 0;
};
