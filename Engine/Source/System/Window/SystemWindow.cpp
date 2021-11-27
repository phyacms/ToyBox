// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindow.h"
#include "ISystemWindowProcedure.h"

FSystemWindow::FSystemWindow(std::unique_ptr<ISystemWindowProcedure> WndProc, FString Title)
	: TObject<FSystemWindow>(*this)
	, WndProc{ std::move(WndProc) }
	, Title{ std::move(Title) }
	, Events{}
{
	if (this->WndProc != nullptr)
	{
		this->WndProc->Initialize(*this);
	}
}

FSystemWindow::~FSystemWindow() noexcept
{
	if (WndProc != nullptr)
	{
		WndProc->Terminate(*this);
	}
}

bool FSystemWindow::IsValid() const noexcept
{
	return WndProc != nullptr
		&& WndProc->IsValid()
		&& &WndProc->GetWindow() == this;
}

FScreenArea FSystemWindow::GetWindowArea() const noexcept
{
	return IsValid()
		? WndProc->GetWindowArea()
		: FScreenArea{};
}

FScreenArea FSystemWindow::GetClientArea() const noexcept
{
	return IsValid()
		? WndProc->GetClientArea()
		: FScreenArea{};
}

void FSystemWindow::Present() noexcept
{
	if (IsValid())
	{
		WndProc->Present();
	}
}

void FSystemWindow::Close() noexcept
{
	if (IsValid())
	{
		WndProc->Close();
	}
}
