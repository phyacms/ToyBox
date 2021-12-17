// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Media/ImageFrame.h"

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
