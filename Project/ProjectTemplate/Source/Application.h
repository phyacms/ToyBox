// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include <Application/IApplication.h>

class FApplication final
	: public IApplication
{
public:
	FApplication();
	virtual ~FApplication() noexcept;

private:
	virtual bool Initialize() noexcept override final;
	virtual void Terminate() noexcept override final;
};
