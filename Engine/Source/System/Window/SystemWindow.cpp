// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "SystemWindow.h"
#include "ISystemWindowProcedure.h"

FSystemWindow::FSystemWindow(std::unique_ptr<ISystemWindowProcedure> WndProc, FStringView Title)
	: WndProc{ std::move(WndProc) }
	, Title{ Title }
{
	if (this->WndProc != nullptr)
	{
		this->WndProc->Initialize(*this);
	}
}

FSystemWindow::~FSystemWindow() noexcept
{
	WndProc->Terminate(*this);
}

bool FSystemWindow::IsValid() const noexcept
{
	return WndProc != nullptr
		&& WndProc->IsValid()
		&& &WndProc->GetWindow() == this;
}

void FSystemWindow::Show() noexcept
{
	if (IsValid())
	{
		WndProc->Show();
	}
}
