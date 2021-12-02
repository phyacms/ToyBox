// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Math/Angle.h"
#include "Type/ScreenSpace/Dim.h"

struct FOrthographicProjection final
{
	UDim Dimension{ UDim::Default };
	float Near{ 0.1f };
	float Far{ 1000.0f };
};

struct FPerspectiveProjectionFoV final
{
	FAngle FieldOfViewY{ Math::PI{} / 4.0f };
	float Near{ 0.1f };
	float Far{ 1000.0f };
};

using FProjection = std::variant<
	FOrthographicProjection,
	FPerspectiveProjectionFoV>;
