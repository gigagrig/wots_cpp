#include "aircraft.hpp"

#include <cassert>
#include <cmath>

#include "ship.hpp"

namespace
{
	const char* toString(AicraftState state)
	{
		switch (state)
		{
		case AicraftState::NotReady:
			return "NotReady";
		case AicraftState::Fueling:
			return "Fueling";
		case AicraftState::Ready:
			return "Ready";
		case AicraftState::MovingToTarget:
			return "MovingToTarget";
		case AicraftState::MovingToBase:
			return "MovingToBase";
		default:
			return "Undefined";
		}
	}

	float turnRadius(float speed, float angularSpeed)
	{
		return fabs(speed / angularSpeed);
	}

	Vector2 centerOfTurn(Vector2 position, float angle, float speed, float angularSpeed)
	{
		const float normalAngle = angularSpeed > 0 ? angle + M_PI / 2 : angle - M_PI / 2;
		Vector2 normal{cosf(normalAngle), sinf(normalAngle)};
		const float r = turnRadius(speed, angularSpeed);
		normal = r*normal;
		const Vector2 center = position + normal;
		return center;
	}

	bool isPointInCircle(Vector2 point, Vector2 center, float r)
	{
		const Vector2 diff = point - center;
		return diff.length() < r - 0.1;
	}

	
}


Aicraft::Aicraft()
	: mesh(nullptr)
{
}

Aicraft::~Aicraft()
{
	deinit();
}

void Aicraft::init(Ship *shiparg, int sideNumber)
{
	assert(!mesh);
	ship = shiparg;
	number = sideNumber;
	state = AicraftState::Ready;
	takticalTask = AicraftTakticalTask::No;
	position = Vector2(0.f, 0.f);
	angle = 0.f;
	acceleration = params::aircraft::ACCELERATION;
}

void Aicraft::deinit()
{
	if (mesh)
	{
		scene::destroyMesh(mesh);
		mesh = nullptr;
	}
}

void Aicraft::update(float dt)
{
	updateState();

	if (state != AicraftState::MovingToTarget && state != AicraftState::MovingToBase)
	{
		return;
	}

	// updating current position
	angle = angle + angularSpeed * dt;
	angle = scopedAngle(angle);

	position = position + speed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
	
	// updatig flight params
	if (speed < params::aircraft::LINEAR_SPEED)
	{
		speed += acceleration * dt;
		if (speed > params::aircraft::LINEAR_SPEED)
		{
			speed = params::aircraft::LINEAR_SPEED;
		}
	}

	Vector2 goalDirection = (state == AicraftState::MovingToTarget) ? 
		target - position :
		ship->GetPosition() - position;

	if (goalDirection.isZero())
	{
		return;
	}

	float targetAngle = std::atan2f(goalDirection.y, goalDirection.x);
	float diff = targetAngle - angle;
	if (isEqual(cosf(diff), 1))
	{
		angularSpeed = 0;
		return;
	}
	if (isAbsEqual(diff, M_PI))
	{
		angularSpeed = params::aircraft::ANGULAR_SPEED;
		return;
	}
	
	float sign = sinf(diff) >= 0 ? 1.f : -1.f;
	
	//float add = add > params::aircraft::ANGULAR_SPEED ? params::aircraft::ANGULAR_SPEED : add;
	float add = params::aircraft::ANGULAR_SPEED;
	angularSpeed = sign * add;

	const float r = turnRadius(speed, angularSpeed);
	const Vector2 center = centerOfTurn(position, angle, speed, angularSpeed);
	Vector2 goal = (state == AicraftState::MovingToTarget) ? target : ship->GetPosition();
	if (isPointInCircle(goal, center, r))
	{
		angularSpeed = - angularSpeed;
	}

}

void Aicraft::launch()
{
	state = AicraftState::MovingToTarget;
	takticalTask = AicraftTakticalTask::MovingToTraget;
	position = ship->GetPosition();
	angle = ship->GetAngle();
	speed = 0;
	angularSpeed = 0;
	nextStateTime = std::chrono::system_clock::now();
	nextStateTime += std::chrono::seconds(params::aircraft::FLIGHT_TIME_SEC);

	mesh = scene::createAircraftMesh();
}

void Aicraft::onLanded()
{
	deinit();
	state = AicraftState::Fueling;
	nextStateTime = std::chrono::system_clock::now();
	nextStateTime += std::chrono::seconds(params::aircraft::FUELING_TIME_SEC);
}

void Aicraft::newTarget(Vector2 worldPosition)
{
	target = worldPosition;
}

bool Aicraft::onTakeoff()
{
	return speed < params::aircraft::LINEAR_SPEED;;
}

#include <cstdio>
void Aicraft::updateState()
{
	auto originalState = state;
	const Vector2 shipVector = ship->GetPosition() - position;
	switch (state)
	{
	case AicraftState::NotReady:
		break;
	case AicraftState::Fueling:
		if (std::chrono::system_clock::now() > nextStateTime)
		{
			state = AicraftState::Ready;
		}
		break;
	case AicraftState::Ready:
		break;
	case AicraftState::MovingToTarget:
		if (isTimeToGoToBase())
		{
			state = AicraftState::MovingToBase;
		}
		break;
	case AicraftState::MovingToBase:
		if (shipVector.length() < 0.1)
		{
			onLanded();
		}
		break;
	default:
		break;
	}
	if (state != originalState)
	{
		GAME_LOG(game::LOG_INFO, "%d state changed:  %s -> %s", number, toString(originalState), toString(state));
		//printf("%d state changed:  %s -> %s\n", number, toString(originalState), toString(state));
	}
}

bool Aicraft::isTimeToGoToBase()
{
	float circleLength = 2.f*M_PI * turnRadius(speed, angularSpeed);
	Vector2 shipDirection(ship->GetPosition().x - position.x, ship->GetPosition().y - position.y);
	float distance = circleLength + shipDirection.length();
	float needTime = distance / fabs(speed);
	auto estimatedArrival =  std::chrono::system_clock::now();
	estimatedArrival += std::chrono::microseconds(static_cast<int>(needTime * 1000000.f));
	estimatedArrival += std::chrono::seconds(1);
	if (estimatedArrival > nextStateTime)
		return true;
	return false;
}
