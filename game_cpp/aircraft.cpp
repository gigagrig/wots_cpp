#include "aircraft.hpp"

#include <cassert>
#include <cmath>

#include "ship.hpp"

namespace
{
	constexpr float POS_EPS = 0.1f;

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
		case AicraftState::Takeoff:
			return "Takeoff";
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
		const float normalAngle = angularSpeed > 0 ? angle + math::PI / 2 : angle - math::PI / 2;
		Vector2 normal{cosf(normalAngle), sinf(normalAngle)};
		const float r = turnRadius(speed, angularSpeed);
		normal = r*normal;
		const Vector2 center = position + normal;
		return center;
	}

	bool isPointInCircle(Vector2 point, Vector2 center, float r)
	{
		const Vector2 diff = point - center;
		return diff.length() <= r;
	}
	
}


Aicraft::Aicraft()
	: mesh(nullptr)
{
}

Aicraft::~Aicraft()
{
	removeMesh();
}

void Aicraft::init(Ship *shiparg, int sideNumber)
{
	assert(!mesh);
	ship = shiparg;
	number = sideNumber;
	setState(AicraftState::Ready);
	acceleration = params::aircraft::ACCELERATION;
	flybyRadius = turnRadius(params::aircraft::LINEAR_SPEED, params::aircraft::ANGULAR_SPEED) +
					params::aircraft::FLYBY_DISTANCE * number;
}

void Aicraft::removeMesh()
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
	if (state != AicraftState::MovingToTarget && state != AicraftState::MovingToBase && state != AicraftState::Takeoff)
	{
		return;
	}

	updateFlightParams(dt);
	updatePosition(dt);
}

void Aicraft::launch()
{
	setState(AicraftState::Takeoff);
	position = ship->getPosition();
	shipPosition = 0;
	angle = ship->getAngle();
	speed = 0;
	angularSpeed = 0;
	nextStateTime = std::chrono::system_clock::now();
	nextStateTime += std::chrono::seconds(params::aircraft::FLIGHT_TIME_SEC);

	mesh = scene::createAircraftMesh();
}

void Aicraft::onLanded()
{
	removeMesh();
	setState(AicraftState::Fueling);
	auto time = std::chrono::system_clock::now();
	if (time > nextStateTime)
	{
		std::chrono::milliseconds delay = std::chrono::duration_cast<std::chrono::milliseconds>(time - nextStateTime);
		GAME_LOG(game::LOG_ERROR, "Aicraft % i is late for %lli ms", number, delay.count());
	}
	nextStateTime = std::chrono::system_clock::now();
	nextStateTime += std::chrono::seconds(params::aircraft::FUELING_TIME_SEC);
}

void Aicraft::newTarget(Vector2 targetPosition)
{
	target = targetPosition;
}

void Aicraft::updateState()
{
	const Vector2 shipVector = ship->getPosition() - position;
	switch (state)
	{
	case AicraftState::NotReady:
		break;
	case AicraftState::Fueling:
		if (std::chrono::system_clock::now() > nextStateTime)
		{
			setState(AicraftState::Ready);
		}
		break;
	case AicraftState::Ready:
		break;
	case AicraftState::Takeoff:
		if (!ship->isOnShip(shipPosition))
		{
			angle = ship->getAngle();
			angularSpeed = 0;
			setState(AicraftState::MovingToTarget);
		}
		break;
	case AicraftState::MovingToTarget:
		if (isTimeToGoToBase())
		{
			setState(AicraftState::MovingToBase);
		}
		break;
	case AicraftState::MovingToBase:
		if (shipVector.length() < POS_EPS)
		{
			onLanded();
		}
		break;
	default:
		break;
	}
}

void Aicraft::updatePosition(float dt)
{
	if (state == AicraftState::Takeoff)
	{
		shipPosition += speed * dt;
		position = ship->localToGlobal(shipPosition);
		angle = ship->getAngle();
		scene::placeMesh(mesh, position.x, position.y, angle);
		return;
	}

	angle = angle + angularSpeed * dt;
	angle = math::scopedAngle(angle);

	position = position + speed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
}

void Aicraft::updateFlightParams(float dt)
{
	if (speed < params::aircraft::LINEAR_SPEED)
	{
		speed += acceleration * dt;
		if (speed > params::aircraft::LINEAR_SPEED)
		{
			speed = params::aircraft::LINEAR_SPEED;
		}
	}

	switch (state)
	{
	case AicraftState::MovingToTarget:
		adjustTrajectoryToMoveAroundTarget(target);
		break;
	case AicraftState::MovingToBase:
		adjustTrajectoryToTarget(ship->getPosition());
		break;
	default:
		break;
	} 
}

void Aicraft::setState(AicraftState newState)
{
	if (state != newState)
	{
		GAME_LOG(game::LOG_INFO, "Aicraft %d state changed:  %s -> %s", number, toString(state), toString(newState));
		state = newState;
	}
}

bool Aicraft::isTimeToGoToBase() const
{
	// rough(but not too) top estimate
	const float circleLength = 2.f*math::PI * turnRadius(speed, angularSpeed);
	const Vector2 shipDirection(ship->getPosition().x - position.x, ship->getPosition().y - position.y);
	const float distance = circleLength + shipDirection.length();
	const float needTime = distance / fabs(speed);
	auto estimatedArrival =  std::chrono::system_clock::now();
	estimatedArrival += std::chrono::microseconds(static_cast<int>(needTime * 1000000.f));
	estimatedArrival += std::chrono::seconds(1);
	if (estimatedArrival > nextStateTime)
		return true;
	return false;
}

void Aicraft::adjustTrajectoryToTarget(Vector2 target)
{
	const Vector2 targetDirection = target - position;

	if (targetDirection.isZero())
	{
		return;
	}

	const float targetAngle = std::atan2f(targetDirection.y, targetDirection.x);
	const float diff = targetAngle - angle;
	if (math::isEqual(cosf(diff), 1))
	{
		angularSpeed = 0;
		return;
	}
	if (math::isAbsEqual(diff, math::PI))
	{
		angularSpeed = params::aircraft::ANGULAR_SPEED;
		return;
	}

	const float sign = sinf(diff) >= 0 ? 1.f : -1.f;
	angularSpeed = sign * params::aircraft::ANGULAR_SPEED;

	// we need adjust trajectory in case target is inside of our turn
	const float r = turnRadius(speed, angularSpeed);
	const Vector2 center = centerOfTurn(position, angle, speed, angularSpeed);
	if (isPointInCircle(target, center, r - POS_EPS))
	{
		angularSpeed = -angularSpeed;
	}
}

void Aicraft::adjustTrajectoryToMoveAroundTarget(Vector2 target)
{
	const float r = flybyRadius;
	if (isPointInCircle(position, target, r-POS_EPS)) // adjusting circle trajectory if aicraft get inside circle
	{
		angularSpeed = 0;
		return;
	}

	// Moving to target circle
	const Vector2 direction = target - position;
	const float targetAngle = atan2f(direction.y, direction.x);
	const float directionTangentAngle = asinf(r / direction.length());
	if (math::isZero(directionTangentAngle))
	{
		angularSpeed = 0;
		return;
	}

	const float desiredAngle = cosf(targetAngle - directionTangentAngle - angle) > cosf(targetAngle + directionTangentAngle - angle) ?
		targetAngle - directionTangentAngle : 
		targetAngle + directionTangentAngle;

	const float diff = desiredAngle - angle;
	if (math::isEqual(cosf(diff), 1))
	{
		angularSpeed = 0;
		return;
	}
	const float sign = sinf(diff) >= 0 ? 1.f : -1.f;
	angularSpeed = sign * params::aircraft::ANGULAR_SPEED;
}
