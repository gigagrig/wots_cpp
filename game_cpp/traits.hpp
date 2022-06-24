#pragma once

#include <cmath>

//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.5f;
		constexpr float ANGULAR_SPEED = 0.5f;
		constexpr int AICRAFTS_COUNT = 5;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.5f;
		constexpr float ACCELERATION = 0.2f;
		constexpr float ANGULAR_SPEED = 0.25*2*3.14f;
		constexpr int FLIGHT_TIME_SEC = 5;
		constexpr int FUELING_TIME_SEC = 10;
	}
}

constexpr float M_PI = 3.14159265358979323846f; // pi
constexpr float M_EPSILON = 0.000001f;

inline float scopedAngle(float angle)
{
	angle = std::fmodf(angle, 2 * M_PI);
	if (angle < 0)
		angle += 2 * M_PI;
	return angle;
}


inline bool isZero(float value)
{
	return fabs(value) < M_EPSILON;
}

inline bool isEqual(float value1, float value2)
{
	return isZero(value1 - value2);
}

inline bool isAbsEqual(float value1, float value2)
{
	return isZero(fabs(value1) - fabs(value2));
}


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

	inline bool isZero() const { return ::isZero(x) && ::isZero(y); }
};

inline Vector2 operator + (Vector2 const &left, Vector2 const &right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}


inline Vector2 operator * (float left, Vector2 const &right)
{
	return Vector2(left * right.x, left * right.y);
}