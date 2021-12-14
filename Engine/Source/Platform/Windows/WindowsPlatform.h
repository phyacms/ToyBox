// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#ifndef _WINDOWS
#define _WINDOWS
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shellapi.h>
#include <wincodec.h>
#ifndef NDEBUG
#include <crtdbg.h>
#endif
#include <wrl.h>

#ifdef DefWindowProc
#undef DefWindowProc
#endif
#ifdef RegisterClass
#undef RegisterClass
#endif
#ifdef UnregisterClass
#undef UnregisterClass
#endif
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef LoadImage
#undef LoadImage
#endif

template<typename T>
using TComPtr = Microsoft::WRL::ComPtr<T>;

enum class EKeyboardKey : std::size_t;
enum class EMouseButton : std::size_t;

namespace WindowsPlatform
{
	class IWindowInterface
	{
	public:
		virtual HWND GetHandle() const noexcept = 0;
	};

	HINSTANCE GetApplicationHandle() noexcept;
	DWORD GetMessageThreadId() noexcept;

	EKeyboardKey TranslateKeyboardKey(int VirtualKey) noexcept;
	EMouseButton TranslateMouseButton(int VirtualKey) noexcept;
}

#endif
