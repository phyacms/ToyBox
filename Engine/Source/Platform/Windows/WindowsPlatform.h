﻿// Copyrights 2021 by phyacms. All Rights Reserved.

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

#include "Platform/IPlatform.h"

template<typename T>
using TComPtr = Microsoft::WRL::ComPtr<T>;

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
	virtual void PrintDebugOutput(FStringView Message) const noexcept override final;
	inline virtual void ShowPopupMessage(FStringView Title, FStringView Content) const noexcept override final
	{
		::MessageBoxW(
			nullptr,
			reinterpret_cast<LPCWSTR>(Content.GetStr()),
			reinterpret_cast<LPCWSTR>(Title.GetStr()),
			MB_OK);
	}
	virtual [[nodiscard]] std::unique_ptr<ISystemWindowProcedure> CreateWindowProcedure() const noexcept override final;

	virtual std::optional<std::vector<char8_t>> StringToUTF8(const FString& Str) const override final;
	virtual std::optional<FString> UTF8ToString(const std::vector<char8_t>& u8Chars) const override final;

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
