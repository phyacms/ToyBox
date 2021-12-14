// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "IImageDecoder.h"

class FSTBImageDecoder final
	: public IImageDecoder
{
public:
	FSTBImageDecoder() = default;
	FSTBImageDecoder(const FSTBImageDecoder&) = default;
	FSTBImageDecoder& operator=(const FSTBImageDecoder&) & = default;
	FSTBImageDecoder(FSTBImageDecoder&&) noexcept = default;
	FSTBImageDecoder& operator=(FSTBImageDecoder&&) & noexcept = default;
	virtual ~FSTBImageDecoder() noexcept = default;

public:
	inline virtual bool IsValid() const noexcept override final { return true; }
	virtual FImageFrame LoadImage(const fs::path& ImageFilePath) const noexcept override final;
};
