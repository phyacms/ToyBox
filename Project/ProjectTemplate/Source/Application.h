// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include <Type/UniqueId.h>
#include <Type/MulticastDelegate.h>
#include <Application/IApplication.h>
#include <System/Input/IInputController.h>

class FSystemWindow;
class FInputContext;
class IGraphicsContext;
class FHUD;

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

	std::unique_ptr<FInputContext> Input;
	std::unique_ptr<IGraphicsContext> Graphics;

	std::unique_ptr<FHUD> HUD;

	ADelegateHandle DH_OnClosed;
	AInputController IC_Application;
	AInputController IC_HUD;
};
