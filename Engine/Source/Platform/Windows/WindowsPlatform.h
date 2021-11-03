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
#include <objbase.h>
#include <shellapi.h>
#ifndef NDEBUG
#include <crtdbg.h>
#endif

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

#include "Platform/IPlatform.h"

class FWindowsPlatform final
	: public IPlatform
{
public:
	static IPlatform& GetInterface() noexcept;
	static FWindowsPlatform& GetSpecific() noexcept;

private:
	FWindowsPlatform() = default;
	virtual ~FWindowsPlatform() noexcept = default;

private:
	inline virtual void RequestAppExit(std::int32_t ExitCode) const noexcept override final
	{
		::PostThreadMessageW(
			GetMessageThreadId(),
			WM_QUIT,
			static_cast<WPARAM>(ExitCode),
			LPARAM{});
	}
	inline virtual void ShowPopupMessage(FStringView Title, FStringView Content) const noexcept override final
	{
		::MessageBoxW(
			nullptr,
			reinterpret_cast<LPCWSTR>(Content.data()),
			reinterpret_cast<LPCWSTR>(Title.data()),
			MB_OK);
	}

	virtual [[nodiscard]] std::unique_ptr<ISystemWindowProcedure> CreateWindowProcedure() const noexcept override final;

public:
	HINSTANCE GetApplicationHandle() const noexcept;
	DWORD GetMessageThreadId() const noexcept;
};

namespace WindowsPlatform
{
	class IWindowInterface
	{
	public:
		virtual HWND GetHandle() const noexcept = 0;
	};
}

#endif
