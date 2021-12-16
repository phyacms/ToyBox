// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Byte.h"

struct FImageFrame final
{
	FBytes RGBAs{};
	std::size_t SysMemRowPitch{};

	inline bool IsValid() const noexcept {
		return SysMemRowPitch != 0
			&& !RGBAs.IsEmpty()
			&& RGBAs.GetByteSize() % (SysMemRowPitch) == 0; }
	template<typename T = std::size_t> inline T GetByteSize() const noexcept { return static_cast<T>(RGBAs.GetByteSize()); }

	template<typename T = std::size_t> inline T Width() const noexcept { return RowPitch<T>() / 4; }
	template<typename T = std::size_t> inline T Height() const noexcept { return GetByteSize<T>() / RowPitch<T>(); }
	template<typename T = std::size_t> inline T RowPitch() const noexcept { return static_cast<T>(SysMemRowPitch); }

	Byte* GetPtr() noexcept { return RGBAs.GetPtr(); }
	const Byte* GetPtr() const noexcept { return RGBAs.GetPtr(); }
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
