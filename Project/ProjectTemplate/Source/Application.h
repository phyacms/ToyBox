// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include <Type/UniqueId.h>
#include <Type/MulticastDelegate.h>
#include <Application/IApplication.h>
#include <System/Input/IInputController.h>

class FSystemWindow;
class FInputContext;
class IGraphicsContext;

class FApplication final
	: public IApplication
	, public IInputController
{
public:
	explicit FApplication(FSystem& System);
	virtual ~FApplication() noexcept;

private:
	virtual bool Initialize(const FCommandLineArgs& CmdLine) noexcept override final;
	virtual void BindInputActions(FInputActionBindings& Actions) override final;
	virtual void Terminate() noexcept override final;

	void Tick(FTimeDuration DeltaTime) override final;
	void Render(FTimeDuration DeltaTime) override final;

private:
	std::unique_ptr<FSystemWindow> Window;
	ADelegateHandle DH_OnClosed;

	std::unique_ptr<FInputContext> Input;
	AInputController IC_Input;

	std::unique_ptr<IGraphicsContext> Graphics;
};
