// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "HUD.h"
#include "System/Input/InputContext.h"
#include "System/Graphics/IGraphicsContext.h"

const FString& FHUD::FRootWidget::GetWidgetName() const noexcept
{
	static const FString WidgetName{ USTR("Widgets") };
	return WidgetName;
}

FHUD::FHUD(
	FInputContext& Input,
	IGraphicsContext& Graphics,
	FAspectRatio AspectRatio)
	: Input{ Input }
	, Graphics{ Graphics }
	, Area{ std::move(AspectRatio) }
	, DH_OnViewportChanged{}
	, Widgets{}
{
	DH_OnViewportChanged = Graphics.OnViewportChanged.AddDynamic(
		[this](const FScreenArea& ViewportArea)->void {
			Area.UpdateArea(ViewportArea);
			UpdateArea(); });

	UpdateArea();
}

FHUD::~FHUD() noexcept
{
	Input.Release();
	Graphics.Release();
}

bool FHUD::IsValid() const noexcept
{
	return Input.IsValid()
		&& Graphics.IsValid();
}

void FHUD::SetDimension(UDim Dimension) noexcept
{
	Area.SetDimension(Dimension, Graphics->GetViewportArea());

	UpdateArea();
}

void FHUD::Render(FTimeDuration DeltaTime)
{
	Widgets.Render(*Graphics.GetAddress(), DeltaTime);
}

void FHUD::UpdateArea() noexcept
{
	Widgets.CalcAbsoluteArea(GetArea());
}

UCoord FHUD::GetMouseCursorLocation() const noexcept
{
	return UAbsCoord{
		FScreenLocation{} + (Input->GetMouseCursorLocation() - GetArea().Location) };
}

bool FHUD::DispatchInputAction(
	const FInputContext& Context,
	FTimePoint Time,
	const FInputTrigger& Trigger) const
{
	// @TODO: HUD input.

	return false;
}
