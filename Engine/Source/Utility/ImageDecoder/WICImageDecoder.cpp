// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WICImageDecoder.h"

#ifdef PLATFORM_WINDOWS

const WICPixelFormatGUID& FWICImageDecoder::GetDefaultWICFormat() noexcept
{
	return GUID_WICPixelFormat32bppRGBA;
}

FWICImageDecoder::FWICImageDecoder()
	: Factory{}
{
	if (FAILED(::CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX::CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&Factory))))
	{
		Factory.Reset();
	}
}

FImageFrame FWICImageDecoder::LoadSingleImageFrame(
	const fs::path& ImageFilePath,
	std::size_t FrameIndex) const noexcept
{
	if (!IsValid())
	{
		return {};
	}

	TComPtr<IWICBitmapDecoder> Decoder{};
	if (FAILED(Factory->CreateDecoderFromFilename(
		ImageFilePath.generic_wstring().c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeOptions::WICDecodeMetadataCacheOnDemand,
		&Decoder)))
	{
		return {};
	}

	UINT FrameCount{};
	if (FAILED(Decoder->GetFrameCount(&FrameCount))
		|| FrameCount == 0
		|| FrameIndex >= FrameCount)
	{
		return {};
	}

	TComPtr<IWICBitmapFrameDecode> Decoded{};
	if (FAILED(Decoder->GetFrame(static_cast<UINT>(FrameIndex), &Decoded)))
	{
		return {};
	}

	return DecodeFrame(*Decoded.Get());
}

FImageFrame FWICImageDecoder::DecodeFrame(IWICBitmapFrameDecode& Decoded) const noexcept
{
	UINT Width{};
	UINT Height{};
	WICPixelFormatGUID PixelFormat{};
	if (FAILED(Decoded.GetSize(&Width, &Height)) ||
		FAILED(Decoded.GetPixelFormat(&PixelFormat)))
	{
		return {};
	}

	IWICBitmapSource* BitmapSource{};
	TComPtr<IWICFormatConverter> Converter{};
	if (const auto bConvertFormat{ std::memcmp(
			&PixelFormat,
			&GetDefaultWICFormat(),
			sizeof(WICPixelFormatGUID)) != 0 };
		bConvertFormat)
	{
		if (SUCCEEDED(Factory->CreateFormatConverter(&Converter)))
		{
			if (BOOL Convertible{};
				SUCCEEDED(Converter->CanConvert(PixelFormat, GetDefaultWICFormat(), &Convertible))
				&& Convertible != FALSE)
			{
				if (SUCCEEDED(Converter->Initialize(
					&Decoded,
					GetDefaultWICFormat(),
					WICBitmapDitherType::WICBitmapDitherTypeErrorDiffusion,
					nullptr,
					0,
					WICBitmapPaletteType::WICBitmapPaletteTypeMedianCut)))
				{
					BitmapSource = Converter.Get();
				}
			}
		}
	}
	else
	{
		BitmapSource = &Decoded;
	}

	if (BitmapSource == nullptr)
	{
		return {};
	}

	const auto RowPitch{ static_cast<std::size_t>(Width) * 4 };
	FImageFrame Frame{
		.RGBAs{ FBytes{ RowPitch * Height } },
		.SysMemRowPitch{ RowPitch } };

	if (FAILED(BitmapSource->CopyPixels(
		nullptr,
		Frame.RowPitch<UINT>(),
		Frame.GetByteSize<UINT>(),
		Frame.GetPtr())))
	{
		return {};
	}

	return Frame;
}

#endif
