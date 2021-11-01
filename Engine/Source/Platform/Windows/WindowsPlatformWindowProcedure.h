// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatform.h"
#include "System/Window/ISystemWindowProcedure.h"

namespace WindowsPlatform::Detail
{
	class FWndProc final
		: public ISystemWindowProcedure
		, public WindowsPlatform::IWindowInterface
	{
	private:
		static constexpr LPCWSTR ClassName{ TEXT("WndClassEx_ToyBox_SystemWindow") };
		static std::size_t RegisterCounter;

	public:
		FWndProc();
		virtual ~FWndProc() noexcept;

	public:
		virtual bool IsValidImpl() const noexcept override final;
		virtual bool InitializeImpl(FSystemWindow& OwnerWindow) noexcept override final;
		virtual void TerminateImpl(FSystemWindow& OwnerWindow) noexcept override final;

		virtual void Show() noexcept override final;

		inline virtual HWND GetHandle() const noexcept override final { return hWnd; }

	private:
		bool RegisterClass() noexcept;
		bool CreateWindow() noexcept;

		void UnregisterClass() noexcept;
		void DestroyWindow() noexcept;

	public:
		static LRESULT CALLBACK DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		LRESULT ProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		bool bRegistered;
		HWND hWnd;
	};
}

#endif
