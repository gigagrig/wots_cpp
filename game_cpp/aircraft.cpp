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

	if (state != AicraftState::MovingToTarget)
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

	Vector2 goalDirection(target.x - position.x, target.y - position.y);
	if (goalDirection.isZero())
	{
		return;
	}

	float targetAngle = std::atan2f(goalDirection.y, goalDirection.x);
	//targetAngle = scopedAngle(targetAngle);
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
	float add = fabsf(diff);
	if (add > M_PI)
	{
		add = 2*M_PI - add;
	}

	
	//add = add > params::aircraft::ANGULAR_SPEED ? params::aircraft::ANGULAR_SPEED : add;
	add = params::aircraft::ANGULAR_SPEED;
	angularSpeed = sign * add;
}

void Aicraft::launch()
{
	state = AicraftState::MovingToTarget;
	takticalTask = AicraftTakticalTask::MovingToTraget;
	position = ship->GetPosition();
	angle = ship->GetAngle();
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
		if (std::chrono::system_clock::now() > nextStateTime)
		{
			onLanded();
		}
		break;
	case AicraftState::MovingToBase:
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
