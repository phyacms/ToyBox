// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "HUD.h"

FHUD::FHUD(
	AObject<FInputContext>&& Input,
	AObject<IGraphicsContext>&& Graphics,
	float MinimumAspectRatio,
	float MaximumAspectRatio)
	: IInputController()
	, Input{ std::move(Input) }
	, Graphics{ std::move(Graphics) }
	, DH_OnViewportAreaChanged{}
	, MinimumAspectRatio(MinimumAspectRatio)
	, MaximumAspectRatio(MaximumAspectRatio)
	, ViewportArea{ this->Graphics->GetViewportArea() }
	, UIArea{}
{
	DH_OnViewportAreaChanged = this->Graphics->OnViewportAreaChanged.AddDynamic(
		[this](const FScreenArea& ViewportArea)->void { this->ViewportArea = ViewportArea; UpdateUIArea(); });

	UpdateUIArea();
}

FHUD::~FHUD() noexcept
{
	DH_OnViewportAreaChanged.Release();
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

	UpdateUIArea();
}

void FHUD::Render()
{
	// @TODO: Render HUD.

	// @TEST: Visualize UI area.
	Graphics->AddCommand(
		[this](const FTimeDuration& DeltaTime)->void
		{
			Graphics->GetSurface().DrawRect(
				UIArea, {
					.Color{ ColorCodes::Green },
					.Width{ 3.0f } });
		});
}

void FHUD::UpdateUIArea() noexcept
{
	UIArea = ViewportArea;
	auto& X{ UIArea.Location.X() };
	auto& Y{ UIArea.Location.Y() };
	auto& Width{ UIArea.Size.X() };
	auto& Height{ UIArea.Size.Y() };

	const auto AspectRatio{ Width / static_cast<float>(Height) };
	if (AspectRatio > MaximumAspectRatio)
	{
		const auto dW{ static_cast<FScreenSize::ValueType>(Height * MaximumAspectRatio) };
		X = static_cast<FScreenLocation::ValueType>(Width - dW) / 2;
		Width = dW;
	}
	else if (AspectRatio < MinimumAspectRatio)
	{
		const auto dH{ static_cast<FScreenSize::ValueType>(Width / MinimumAspectRatio) };
		Y = static_cast<FScreenLocation::ValueType>(Height - dH) / 2;
		Height = dH;
	}
}
