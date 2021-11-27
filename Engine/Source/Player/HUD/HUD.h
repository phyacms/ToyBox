// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Time.h"
#include "Type/Delegate/DelegateHandle.h"
#include "System/Window/ScreenSpace.h"
#include "System/Input/Controller/IInputController.h"
#include "Widget/IWidget.h"
#include "UISpace.h"

class FInputContext;
class IGraphicsContext;

class FHUD final
	: public IInputController
{
private:
	class FRootWidget final
		: public IWidget
	{
	public:
		FRootWidget() : IWidget() {}

	public:
		using IWidget::CalcAbsoluteArea;

	private:
		virtual const FString& GetWidgetName() const noexcept override final;
		inline virtual bool IsValidImpl() const noexcept override final { return true; }
		virtual void RenderImpl(ISurface2D& Surface, const FScreenArea& Rect, FTimeDuration DeltaTime) override final {}
	};

public:
	inline static constexpr float DefaultMinimumAspectRatio{ 9.0f / 16.0f };
	inline static constexpr float DefaultMaximumAspectRatio{ 48.0f / 9.0f };

public:
	FHUD(
		FInputContext& Input,
		IGraphicsContext& Graphics,
		float MinimumAspectRatio = DefaultMinimumAspectRatio,
		float MaximumAspectRatio = DefaultMaximumAspectRatio);
	~FHUD() noexcept;

	FHUD(const FHUD&) = delete;
	FHUD& operator=(const FHUD&) & = delete;
	FHUD(FHUD&&) noexcept = delete;
	FHUD& operator=(FHUD&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;
	inline const FScreenArea& GetUIArea() const noexcept { return UIArea; }

	void SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept;

	// @NOTE: Note that [[nodiscard]] attribute is NOT enforced here.
	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_base_of_v<IWidget, T>>>
	inline AWidget<T> CreateWidget(Ts&&... Params) { return Root.CreateChild<T>(std::forward<Ts>(Params)...); }

	void Render();

private:
	void UpdateUIArea() noexcept;

	UCoord GetMouseCursorLocation() const noexcept;

	virtual bool DispatchInputAction(
		const FInputContext& Context,
		FTimePoint Time,
		const FInputTrigger& Trigger) const override final;

private:
	AObject<FInputContext> Input;
	AObject<IGraphicsContext> Graphics;

	FScreenArea ViewportArea{};
	ADelegateHandle DH_OnViewportAreaChanged;

	float MinimumAspectRatio;
	float MaximumAspectRatio;
	FScreenArea UIArea;

	FRootWidget Root;
};
