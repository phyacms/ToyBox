// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "HUD.h"
#include "System/Input/InputContext.h"
#include "System/Graphics/IGraphicsContext.h"

const FString& FHUD::FRootWidget::GetWidgetName() const noexcept
{
	static const FString WidgetName{ USTR("Root") };
	return WidgetName;
}

FHUD::FHUD(
	FInputContext& Input,
	IGraphicsContext& Graphics,
	URect Viewport,
	float MinimumAspectRatio,
	float MaximumAspectRatio)
	: Input{ Input }
	, Graphics{ Graphics }
	, Viewport{ std::move(Viewport) }
	, OutputWindowSize{ Graphics.GetOutputWindowSize() }
	, DH_OnOutputWindowSizeChanged{}
	, MinimumAspectRatio(MinimumAspectRatio)
	, MaximumAspectRatio(MaximumAspectRatio)
	, Area{}
	, Root{}
{
	DH_OnOutputWindowSizeChanged = this->Graphics->OnOutputWindowSizeChanged.AddDynamic(
		[this](const FScreenSize& OutputWindowSize)->void {
			this->OutputWindowSize = OutputWindowSize;
			UpdateArea(); });

	UpdateArea();
}

FHUD::~FHUD() noexcept
{
	DH_OnOutputWindowSizeChanged.Release();
	Input.Release();
	Graphics.Release();
}

bool FHUD::IsValid() const noexcept
{
	return Input.IsValid()
		&& Graphics.IsValid();
}

void FHUD::SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept
{
	// Sanitize arguments.
	if (MinimumAspectRatio < 0.0f) { MinimumAspectRatio = DefaultMinimumAspectRatio; }
	if (MaximumAspectRatio < 0.0f) { MaximumAspectRatio = DefaultMaximumAspectRatio; }
	if (MaximumAspectRatio < MinimumAspectRatio) { MaximumAspectRatio = MinimumAspectRatio; }

	this->MinimumAspectRatio = MinimumAspectRatio;
	this->MaximumAspectRatio = MaximumAspectRatio;

	UpdateArea();
}

void FHUD::Render(FTimeDuration DeltaTime)
{
	Root.Render(*Graphics.GetAddress(), DeltaTime);
}

void FHUD::UpdateArea() noexcept
{
	Area = ::ToScreenSpace(Viewport, { .Location{}, .Size{ OutputWindowSize } });
	auto& X{ Area.Location.X() };
	auto& Y{ Area.Location.Y() };
	auto& Width{ Area.Size.X() };
	auto& Height{ Area.Size.Y() };

	const auto AspectRatio{ Width / static_cast<float>(Height) };
	if (AspectRatio > MaximumAspectRatio)
	{
		const auto dW{ static_cast<FScreenSize::ValueType>(Height * MaximumAspectRatio) };
		X += static_cast<FScreenLocation::ValueType>(Width - dW) / 2;
		Width = dW;
	}
	else if (AspectRatio < MinimumAspectRatio)
	{
		const auto dH{ static_cast<FScreenSize::ValueType>(Width / MinimumAspectRatio) };
		Y += static_cast<FScreenLocation::ValueType>(Height - dH) / 2;
		Height = dH;
	}

	Root.CalcAbsoluteArea(Area);
}

UCoord FHUD::GetMouseCursorLocation() const noexcept
{
	return UAbsCoord{
		FScreenLocation()
		+ (Input->GetMouseCursorLocation() - Area.Location) };
}

bool FHUD::DispatchInputAction(
	const FInputContext& Context,
	FTimePoint Time,
	const FInputTrigger& Trigger) const
{
	return false;
}
