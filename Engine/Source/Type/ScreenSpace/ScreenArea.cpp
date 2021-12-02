// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "ScreenArea.h"

FScreenArea FScreenArea::GetInnerAreaOf(float MinimumAspectRatio, float MaximumAspectRatio) const noexcept
{
	auto Area{ *this };
	auto& X{ Area.Location.X() };
	auto& Y{ Area.Location.Y() };
	auto& Width{ Area.Size.X() };
	auto& Height{ Area.Size.Y() };

	const auto AspectRatio{ Width / static_cast<float>(Height) };
	if (Math::IsNotEqualTo(MaximumAspectRatio, 0.0f) && AspectRatio > MaximumAspectRatio)
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

	return Area;
}
