// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/TimePoint.h"
#include "Type/Delegate/DelegateHandle.h"
#include "System/Window/ScreenSpace.h"
#include "System/Input/Controller/IInputController.h"
#include "UISpace.h"

class FInputContext;
class IGraphicsContext;

class FHUD final
	: public IInputController
{
public:
	inline static constexpr float DefaultMinimumAspectRatio{ 9.0f / 16.0f };
	inline static constexpr float DefaultMaximumAspectRatio{ 48.0f / 9.0f };

private:
	static const URect UIRect;

public:
	FHUD(
		AObject<FInputContext>&& Input,
		AObject<IGraphicsContext>&& Graphics,
		float MinimumAspectRatio = DefaultMinimumAspectRatio,
		float MaximumAspectRatio = DefaultMaximumAspectRatio);
	~FHUD() noexcept;

	FHUD(const FHUD&) = delete;
	FHUD& operator=(const FHUD&) & = delete;
	FHUD(FHUD&&) noexcept = delete;
	FHUD& operator=(FHUD&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;

	void SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept;

	inline const FScreenArea& GetUIArea() const noexcept { return UIArea; }

	void Render();

private:
	void UpdateUIArea() noexcept;

	UCoord GetMouseCursorLocation() const noexcept;

	virtual bool DispatchKeyboardKeyEvent(const FInputContext& Context, EKeyboardKey Key, ESwitchEvent Event) const override final;
	virtual bool DispatchMouseButtonEvent(const FInputContext& Context, EMouseButton Button, ESwitchEvent Event) const override final;
	virtual bool DispatchMouseWheelMoveEvent(const FInputContext& Context, EMouseWheel Wheel) const override final;

private:
	AObject<FInputContext> Input;
	AObject<IGraphicsContext> Graphics;

	FScreenArea ViewportArea{};
	ADelegateHandle DH_OnViewportAreaChanged;

	float MinimumAspectRatio;
	float MaximumAspectRatio;
	FScreenArea UIArea;
};
