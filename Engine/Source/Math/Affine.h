// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Vector.h"
#include "Point.h"
#include "Matrix.h"
#include "Quaternion.h"

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

	inline Matrix4x4f ToScaleMatrix(const Vector3f& Scale) noexcept
	{
		auto S{ Matrix4x4f::GetIdentity() };
		S.At<0, 0>() = Scale.X();
		S.At<1, 1>() = Scale.Y();
		S.At<2, 2>() = Scale.Z();
		return S;
	}
	inline Matrix4x4f ToRotationMatrix(const FUnitQuaternion& Rotation) { return Rotation.ToMatrix(); }
	inline Matrix4x4f ToRotationMatrix(FAngle Angle, const Vector3f& Axis) { return ToRotationMatrix({ Angle, Axis }); }
	inline Matrix4x4f ToTranslationMatrix(const Point3f& Translation) noexcept
	{
		auto T{ Matrix4x4f::GetIdentity() };
		T.At<0, 3>() = Translation.X();
		T.At<1, 3>() = Translation.Y();
		T.At<2, 3>() = Translation.Z();
		return T;
	}
}
