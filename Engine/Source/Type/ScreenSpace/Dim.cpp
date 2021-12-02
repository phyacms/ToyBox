// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Dim.h"

const UDim UDim::Default{ .Offset{ URelCoord{ 0, 0 } }, .Scale{ URelCoord{ 1, 1 } } };

void FAspectRatio::SetAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio) noexcept
{
	// Sanitize arguments.
	if (MinimumAspectRatio < 0.0f) { MinimumAspectRatio = 0.0f; }
	if (MaximumAspectRatio < 0.0f) { MaximumAspectRatio = 0.0f; }
	if (MaximumAspectRatio < MinimumAspectRatio) { MaximumAspectRatio = MinimumAspectRatio; }

	this->MinimumAspectRatio = MinimumAspectRatio;
	this->MaximumAspectRatio = MaximumAspectRatio;

	UpdateArea();
}
