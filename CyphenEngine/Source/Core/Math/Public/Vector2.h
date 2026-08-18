#pragma once

#include "Core/Math/Public/Mathf.h"

// ============================================================================
// Vector2
// ----------------------------------------------------------------------------
// CyphenEngine 2D 월드 좌표를 표현하는 float 기반 값 타입입니다.
//
// 월드 기본 좌표계:
//     원점은 좌하단입니다.
//     +X는 오른쪽입니다.
//     +Y는 위쪽입니다.
// ============================================================================

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;

	constexpr Vector2() = default;

	constexpr Vector2(float inX, float inY)
		: x(inX)
		, y(inY)
	{
	}

	static constexpr Vector2 Zero()
	{
		return Vector2(0.0f, 0.0f);
	}

	static constexpr Vector2 One()
	{
		return Vector2(1.0f, 1.0f);
	}

	static constexpr Vector2 Right()
	{
		return Vector2(1.0f, 0.0f);
	}

	static constexpr Vector2 Up()
	{
		return Vector2(0.0f, 1.0f);
	}

	constexpr float SqrMagnitude() const
	{
		return x * x + y * y;
	}

	float Magnitude() const
	{
		return Mathf::Sqrt(SqrMagnitude());
	}

	Vector2 Normalized() const
	{
		const float magnitude = Magnitude();

		if (magnitude <= Mathf::Epsilon)
		{
			return Zero();
		}

		return Vector2(x / magnitude, y / magnitude);
	}

	static constexpr float Dot(const Vector2& left, const Vector2& right)
	{
		return left.x * right.x + left.y * right.y;
	}

	// 2D 외적입니다. 3D cross의 z성분에 해당하는 스칼라이며, 부호로 회전 방향을 나타냅니다.
	static constexpr float Cross(const Vector2& left, const Vector2& right)
	{
		return left.x * right.y - left.y * right.x;
	}

	static constexpr float SqrDistance(const Vector2& left, const Vector2& right)
	{
		return (right.x - left.x) * (right.x - left.x)
			+ (right.y - left.y) * (right.y - left.y);
	}

	static float Distance(const Vector2& left, const Vector2& right)
	{
		return Mathf::Sqrt(SqrDistance(left, right));
	}

	// t를 0~1로 강제하지 않는 비클램프 보간입니다. Mathf::Lerp와 동일한 정책입니다.
	static constexpr Vector2 Lerp(const Vector2& from, const Vector2& to, float t)
	{
		return Vector2(
			from.x + (to.x - from.x) * t,
			from.y + (to.y - from.y) * t);
	}

	// +90도(반시계) 회전입니다. +Y up 기준이며 (x, y) -> (-y, x)입니다.
	constexpr Vector2 Perpendicular() const
	{
		return Vector2(-y, x);
	}

	// 성분별 근사 동등 비교입니다. 정확한 operator== 대신 float 오차를 허용합니다.
	static bool Approximately(const Vector2& left, const Vector2& right)
	{
		return Mathf::Approximately(left.x, right.x)
			&& Mathf::Approximately(left.y, right.y);
	}

	Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	Vector2& operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}
};

static_assert(sizeof(Vector2) == sizeof(float) * 2, "Vector2 must remain tight 8-byte storage.");
static_assert(alignof(Vector2) == alignof(float), "Vector2 storage must not add default SIMD alignment.");

constexpr Vector2 operator+(const Vector2& left, const Vector2& right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}

constexpr Vector2 operator-(const Vector2& left, const Vector2& right)
{
	return Vector2(left.x - right.x, left.y - right.y);
}

constexpr Vector2 operator-(const Vector2& vector)
{
	return Vector2(-vector.x, -vector.y);
}

constexpr Vector2 operator*(const Vector2& vector, float scalar)
{
	return Vector2(vector.x * scalar, vector.y * scalar);
}

constexpr Vector2 operator*(float scalar, const Vector2& vector)
{
	return vector * scalar;
}

constexpr Vector2 operator/(const Vector2& vector, float scalar)
{
	return Vector2(vector.x / scalar, vector.y / scalar);
}

constexpr bool operator==(const Vector2& left, const Vector2& right)
{
	return left.x == right.x && left.y == right.y;
}

constexpr bool operator!=(const Vector2& left, const Vector2& right)
{
	return !(left == right);
}
