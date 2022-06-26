#pragma once

#include <cmath>

//-------------------------------------------------------
//	Math utils
//-------------------------------------------------------

namespace math
{
constexpr float PI = 3.14159265358979323846f; // pi
constexpr float EPSILON = 0.00001f;

inline float scopedAngle(float angle)
{
	angle = std::fmodf(angle, 2*math::PI);
	if (angle < 0)
		angle += 2 * math::PI;
	return angle;
}


inline bool isZero(float value)
{
	return fabs(value) < math::EPSILON;
}

inline bool isEqual(float value1, float value2)
{
	return isZero(value1 - value2);
}

inline bool isAbsEqual(float value1, float value2)
{
	return isZero(fabs(value1) - fabs(value2));
}

} // namespace math


//-------------------------------------------------------
//	Basic Vector2 class
//-------------------------------------------------------

class Vector2
{
public:
	float x;
	float y;

	Vector2() :
		x(0.f),
		y(0.f)
	{ }
	Vector2(float vx, float vy) :
		x(vx),
		y(vy)
	{ }

	Vector2(Vector2 const &other) :
		x(other.x),
		y(other.y)
	{ }

	inline bool isZero() const { return math::isZero(x) && math::isZero(y); }
	inline float length() const { return sqrtf(x*x + y* y); }
};

inline Vector2 operator + (Vector2 const &left, Vector2 const &right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}

inline Vector2 operator - (Vector2 const &left, Vector2 const &right)
{
	return Vector2(left.x - right.x, left.y - right.y);
}

inline Vector2 operator * (float left, Vector2 const &right)
{
	return Vector2(left * right.x, left * right.y);
}