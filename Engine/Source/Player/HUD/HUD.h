// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Time.h"
#include "Type/Delegate/DelegateHandle.h"
#include "Type/ScreenSpace/Dim.h"
#include "System/Input/Controller/IInputController.h"
#include "Widget/IWidget.h"

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
		virtual void RenderImpl(IGraphicsContext&, const FScreenArea&, FTimeDuration) override final {}
	};

public:
	FHUD(
		FInputContext& Input,
		IGraphicsContext& Graphics,
		FAspectRatio AspectRatio = {});
	~FHUD() noexcept;

	FHUD(const FHUD&) = delete;
	FHUD& operator=(const FHUD&) & = delete;
	FHUD(FHUD&&) noexcept = delete;
	FHUD& operator=(FHUD&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;

	void SetDimension(UDim Dimension) noexcept;
	inline void SetAspectRatio(float AspectRatio) noexcept { SetAspectRatio(AspectRatio, AspectRatio); }
	inline void SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept { Area.SetAspectRatio(MinimumAspectRatio, MaximumAspectRatio); UpdateArea(); }
	inline const FScreenArea& GetArea() const noexcept { return Area.GetArea(); }

	// @NOTE: Note that [[nodiscard]] attribute is NOT enforced here.
	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_base_of_v<IWidget, T>>>
	inline AWidget<T> CreateWidget(Ts&&... Params)
	{
		auto Widget{ Widgets.CreateChild<T>(std::forward<Ts>(Params)...) };
		if (Widget.IsValid())
		{
			Widget->CalcAbsoluteArea(GetArea());
		}
		return Widget;
	}

	void Render(FTimeDuration DeltaTime);

private:
	void UpdateArea() noexcept;

	UCoord GetMouseCursorLocation() const noexcept;

	virtual bool DispatchInputAction(
		const FInputContext& Context,
		FTimePoint Time,
		const FInputTrigger& Trigger) const override final;

private:
	TObjRef<FInputContext> Input;
	TObjRef<IGraphicsContext> Graphics;

	FAspectRatio Area;
	ADelegateHandle DH_OnViewportChanged;

	FRootWidget Widgets;
};
