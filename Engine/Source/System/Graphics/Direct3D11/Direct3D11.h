// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

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

namespace Direct3D11
{
	inline TComPtr<ID3DBlob> LoadCompiledShaderObject(const std::filesystem::path& Path) noexcept
	{
		TComPtr<ID3DBlob> Blob{};
		return SUCCEEDED(::D3DReadFileToBlob(Path.generic_wstring().c_str(), &Blob))
			? Blob
			: nullptr;
	}
}

#endif
