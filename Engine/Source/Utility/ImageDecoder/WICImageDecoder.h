// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Platform/Windows/WindowsPlatform.h"
#include "IImageDecoder.h"

class FWICImageDecoder final
	: public IImageDecoder
{
private:
	static const WICPixelFormatGUID& GetDefaultWICFormat() noexcept;

public:
	FWICImageDecoder();
	FWICImageDecoder(const FWICImageDecoder&) = default;
	FWICImageDecoder& operator=(const FWICImageDecoder&) & = default;
	FWICImageDecoder(FWICImageDecoder&&) noexcept = default;
	FWICImageDecoder& operator=(FWICImageDecoder&&) & noexcept = default;
	virtual ~FWICImageDecoder() noexcept = default;

public:
	inline virtual bool IsValid() const noexcept override final { return Factory != nullptr; }
	inline virtual FImageFrame LoadImage(const fs::path& ImageFilePath) const noexcept override final {
		return LoadSingleImageFrame(ImageFilePath, 0); }

	FImageFrame LoadSingleImageFrame(const fs::path& ImageFilePath, std::size_t FrameIndex) const noexcept;

private:
	FImageFrame DecodeFrame(IWICBitmapFrameDecode& Decoded) const noexcept;

private:
	TComPtr<IWICImagingFactory> Factory;
};

#endif
