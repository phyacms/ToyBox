// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Vector.h"
#include "Point.h"
#include "Matrix.h"

namespace Affine
{
	using Vector3f = TVector<float, 3>;
	using Point3f = TPoint<float, 3>;
	using Matrix4x4f = TMatrix<float, 4>;

	namespace Operations
	{
		using Vector4f = Matrix4x4f::ColumnVectorType;

		inline Vector3f ToVector3f(const Vector4f& V) noexcept { return Vector3f{ V.X(), V.Y(), V.Z() }; }
		inline Vector4f ToVector4f(const Vector3f& V) noexcept { return Vector4f{ V.X(), V.Y(), V.Z(), 0.0f }; }
		inline Vector4f ToVector4f(const Point3f& P) noexcept { return Vector4f{ P.X(), P.Y(), P.Z(), 1.0f }; }

		inline Vector3f operator*(const Matrix4x4f& M, const Vector3f& V) noexcept { return ToVector3f(M * ToVector4f(V)); }
		inline Point3f operator*(const Matrix4x4f& M, const Point3f& V) noexcept { return Point3f{ ToVector3f(M * ToVector4f(V)) }; }
	}
}
