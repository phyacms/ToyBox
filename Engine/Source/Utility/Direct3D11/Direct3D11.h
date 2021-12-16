// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Platform/Windows/WindowsPlatform.h"

#include <dxgi1_5.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "Type/Color.h"
#include "Type/ScreenSpace/ScreenArea.h"

namespace Direct3D11
{
	inline TComPtr<ID3DBlob> LoadCompiledShaderObject(const fs::path& Path) noexcept
	{
		TComPtr<ID3DBlob> Blob{};
		return SUCCEEDED(::D3DReadFileToBlob(Path.generic_wstring().c_str(), &Blob))
			? Blob
			: nullptr;
	}
}

namespace Direct2D1
{
	inline D2D1_POINT_2F ToPoint(const FScreenLocation& Point) noexcept
	{
		return D2D1::Point2F(
			Point.X<FLOAT>(),
			Point.Y<FLOAT>());
	}

	inline D2D1_RECT_F ToRect(const FScreenArea& Rect) noexcept
	{
		return D2D1::RectF(
			Rect.Location.X<FLOAT>(),
			Rect.Location.Y<FLOAT>(),
			Rect.Location.X<FLOAT>() + Rect.Size.X<FLOAT>(),
			Rect.Location.Y<FLOAT>() + Rect.Size.Y<FLOAT>());
	}

	inline D2D1_COLOR_F ToColor(const FColor& Color) noexcept
	{
		D2D1_COLOR_F D2DColor{};
		std::memcpy(&D2DColor, Color.GetPtr(), sizeof(D2D1_COLOR_F));
		return D2DColor;
	}
}

#endif
