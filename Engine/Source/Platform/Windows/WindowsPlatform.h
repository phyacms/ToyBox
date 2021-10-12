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
#ifndef NDEBUG
#include <crtdbg.h>
#endif

#ifdef TEXT
#undef TEXT
#endif

#include "Platform/IPlatform.h"

class FWindowsPlatform final
	: public IPlatform
{
public:
	static IPlatform& GetInstance() noexcept;
	template<typename T>
	inline static T& GetInstanceAs() noexcept { return dynamic_cast<T>(GetInstance()); }

private:
	FWindowsPlatform() = default;
	virtual ~FWindowsPlatform() noexcept = default;

public:
	inline virtual void RequestAppExit(std::int32_t ExitCode) noexcept override final { ::PostQuitMessage(static_cast<int>(ExitCode)); }

public:
	inline HINSTANCE GetApplicationHandle() const noexcept { return ::GetModuleHandleW(nullptr); }
};

#endif
