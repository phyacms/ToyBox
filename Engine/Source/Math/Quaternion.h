// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Angle.h"
#include "Vector.h"
#include "Matrix.h"

template<typename T>
class TQuaternion final
{
public:
	inline static constexpr auto Dimension{ 4 };
	using ValueType = T;
	using ScalarPartType = ValueType;
	using VectorPartType = TVector<ValueType, Dimension - 1>;

private:
	using VectorType = TVector<ValueType, Dimension>;

public:
	using LengthType = VectorType::LengthType;
	static_assert(std::is_floating_point_v<ValueType>);

public:
	TQuaternion() : TQuaternion(1, {}) {}
	TQuaternion(ScalarPartType W, VectorPartType V) : W{ W }, V{ std::move(V) } {}
	template<
		typename... Parameters,
		typename = std::enable_if_t<sizeof...(Parameters) == Dimension>>
	explicit TQuaternion(Parameters&&... Params) : Components{ static_cast<ValueType>(Params)... } {}
	TQuaternion(const TQuaternion&) = default;
	TQuaternion& operator=(const TQuaternion&) & = default;
	TQuaternion(TQuaternion&&) noexcept = default;
	TQuaternion& operator=(TQuaternion&&) & noexcept = default;
	~TQuaternion() noexcept { static_assert(sizeof(TQuaternion) == sizeof(ValueType) * Dimension); }

	friend inline bool operator==(const TQuaternion& Lhs, const TQuaternion& Rhs) noexcept { return Lhs.Components == Rhs.Components; }
	friend inline bool operator!=(const TQuaternion& Lhs, const TQuaternion& Rhs) noexcept { return Lhs.Components != Rhs.Components; }

	inline TQuaternion operator+() const noexcept { return *this; }
	inline TQuaternion operator-() const noexcept { return operator*(-1); }

	inline TQuaternion operator*(const TQuaternion& Q) const noexcept
	{
		return {
			W * Q.W - V.DotProduct(Q.V),
			W * Q.V + Q.W * V - V.CrossProduct(Q.V) };
	}
	inline TQuaternion operator*(const ValueType& Factor) const noexcept { TQuaternion Q{ *this }; Q *= Factor; return Q; }
	inline TQuaternion operator/(const ValueType& Divisor) const noexcept { TQuaternion Q{ *this }; Q /= Divisor; return Q; }

	inline TQuaternion& operator*=(const TQuaternion& Q) noexcept { return *this = operator*(Q); }
	inline TQuaternion& operator*=(const ValueType& Factor) noexcept { Components *= Factor; return *this; }
	inline TQuaternion& operator/=(const ValueType& Divisor) noexcept { Components /= Divisor; return *this; }

public:
	inline T& ScalarPart() noexcept { return W; }
	inline const T& ScalarPart() const noexcept { return W; }
	inline VectorPartType& VectorPart() noexcept { return V; }
	inline const VectorPartType& VectorPart() const noexcept { return V; }

public:
	inline bool IsIdentity() const noexcept { return Math::TIsEqualTo<ValueType>{}(W, 1) && V.IsZero(); }
	inline bool IsUnit() const noexcept { return Math::TIsEqualTo<ValueType>{}(NormSq(), 1); }
	inline LengthType Norm() const noexcept { return static_cast<T>(std::sqrt(static_cast<LengthType>(NormSq()))); }
	inline T NormSq() const noexcept { return operator*(Conjugation()).W; }
	inline TQuaternion Normalized() const { return operator/(Norm()); }
	inline TQuaternion Reciprocal() const { return Conjugation() / NormSq(); }
	inline TQuaternion Conjugation() const noexcept { TQuaternion Q{ *this }; Q.Conjugate(); return Q; }

	inline TQuaternion& Normalize() { return *this = Normalized(); }
	inline TQuaternion& Reciprocate() { return *this = Reciprocal(); }
	inline TQuaternion& Conjugate() noexcept { V = -V; return *this; }

public:
	inline ValueType* GetPtr() noexcept { return Components.GetPtr(); }
	inline const ValueType* GetPtr() const noexcept { return Components.GetPtr(); }

private:
	union
	{
		VectorType Components;
		struct
		{
			ScalarPartType W;
			VectorPartType V;
		};
	};
};

template<typename T>
inline TQuaternion<T> operator*(const T& Factor, const TQuaternion<T>& Q) noexcept { return Q * Factor; }

template<typename T>
class TUnitQuaternion final
{
private:
	using QuaternionType = TQuaternion<T>;

public:
	inline static constexpr auto Dimension{ QuaternionType::Dimension };

	using ValueType = QuaternionType::ValueType;
	using LengthType = QuaternionType::LengthType;

	using ScalarPartType = QuaternionType::ScalarPartType;
	using VectorPartType = QuaternionType::VectorPartType;
	using MatrixType = TMatrix<ValueType, Dimension>;

private:
	struct FRotationAxisAndAngle final
	{
		VectorPartType Axis{};
		FAngle Angle{};
	};

public:
	TUnitQuaternion() : Q{} {}
	TUnitQuaternion(FAngle Angle, const VectorPartType& Axis)
		: TUnitQuaternion()
	{
		if (!Axis.IsZero())
		{
			Q = QuaternionType{
				static_cast<ValueType>(std::cos(static_cast<Math::Angle::ValueType>(Angle) / 2)),
				static_cast<ValueType>(std::sin(static_cast<Math::Angle::ValueType>(Angle) / 2)) * Axis.Normalized() };
		}
	}
	TUnitQuaternion(const TUnitQuaternion&) = default;
	TUnitQuaternion& operator=(const TUnitQuaternion&) & = default;
	TUnitQuaternion(TUnitQuaternion&&) noexcept = default;
	TUnitQuaternion& operator=(TUnitQuaternion&&) & noexcept = default;
	~TUnitQuaternion() noexcept { static_assert(sizeof(TUnitQuaternion) == sizeof(QuaternionType)); }

private:
	TUnitQuaternion(const QuaternionType& Q) : Q{ Q.Normalized() } {}
	TUnitQuaternion(QuaternionType&& Q) noexcept : Q{ std::move(Q.Normalize()) } {}
	TUnitQuaternion(ScalarPartType W, VectorPartType V) : Q{ W, std::move(V) } {}

public:
	friend inline bool operator==(const TUnitQuaternion& Lhs, const TUnitQuaternion& Rhs) noexcept = default;
	friend inline bool operator!=(const TUnitQuaternion& Lhs, const TUnitQuaternion& Rhs) noexcept = default;

	inline TUnitQuaternion operator+() const noexcept { return Q.operator+(); }
	inline TUnitQuaternion operator-() const noexcept { return Q.operator-(); }

	inline TUnitQuaternion operator*(const TUnitQuaternion& Q) const noexcept { return this->Q.operator*(Q); }

	inline TUnitQuaternion& operator*=(const TUnitQuaternion& Q) noexcept { this->Q.operator*=(Q); return *this; }

	inline operator MatrixType() const noexcept { return ToMatrix(); }

public:
	inline const T& ScalarPart() const noexcept { return Q.W; }
	inline const VectorPartType& VectorPart() const noexcept { return Q.V; }

public:
	inline bool IsIdentity() const noexcept { return Q.IsIdentity(); }
	inline FAngle GetRotationAngle() const noexcept { return static_cast<ValueType>(2 * std::acos(static_cast<Math::Angle::ValueType>(Q.W))); }
	inline VectorPartType GetRotationAxis() const noexcept
	{
		const auto DivSq{ 1 - Q.W * Q.W };
		return Math::IsNotEqualTo<ValueType>(DivSq, 0)
			? VectorPart().Normalized()
			: VectorPartType{ 1, 0, 0 };
	}
	inline TUnitQuaternion Reciprocal() const { return Q.Reciprocal(); }
	inline TUnitQuaternion Conjugation() const noexcept { return Q.Conjugation(); }
	inline TUnitQuaternion Powered(ValueType P) const noexcept
	{
		return !IsIdentity()
			? TUnitQuaternion{ P * GetRotationAngle(), GetRotationAxis() }
			: *this;
	}

	inline TUnitQuaternion& Reciprocate() { Q.Reciprocate(); return *this; }
	inline TUnitQuaternion& Conjugate() noexcept { Q.Conjugate(); return *this; }
	inline TUnitQuaternion& Power(ValueType P) noexcept { return *this = Powered(P); }

public:
	inline MatrixType ToMatrix() const noexcept
	{
		const auto& X{ Q.V.X() };
		const auto& Y{ Q.V.Y() };
		const auto& Z{ Q.V.Z() };
		const auto& W{ Q.W };

		const auto X_Sq{ X * X };
		const auto Y_Sq{ Y * Y };
		const auto Z_Sq{ Z * Z };

		const auto XY{ X * Y };
		const auto YZ{ Y * Z };
		const auto ZX{ Z * X };

		const auto XW{ X * W };
		const auto YW{ Y * W };
		const auto ZW{ Z * W };

		return MatrixType{
			1 - 2 * Y_Sq - 2 * Z_Sq, 2 * XY - 2 * ZW, 2 * ZX + 2 * YW, 0,
			2 * XY + 2 * ZW, 1 - 2 * X_Sq - 2 * Z_Sq, 2 * YZ - 2 * XW, 0,
			2 * ZX - 2 * YW, 2 * YZ + 2 * XW, 1 - 2 * X_Sq - 2 * Y_Sq, 0,
			0, 0, 0, 1 };
	}

	inline const ValueType* GetPtr() const noexcept { return Q.GetPtr(); }

private:
	QuaternionType Q;
};

// Alias for typical use.
using FUnitQuaternion = TUnitQuaternion<float>;
