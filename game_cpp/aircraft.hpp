#pragma once

#include "../framework/scene.hpp"
#include "../framework/game.hpp"
#include "utils.hpp"

#include <memory>
#include <chrono>


//-------------------------------------------------------
//	Aircraft logic
//-------------------------------------------------------

class Aicraft;
typedef std::unique_ptr<Aicraft> AicraftPtr;
class Ship;

enum class AicraftState
{
	NotReady = 0,
	Fueling,
	Ready,
	Takeoff,
	MovingToTarget,
	MovingToBase
};


class Aicraft
{

public:
	Aicraft();
	~Aicraft();

	void init(Ship *ship, int sideNumber);
	AicraftState getState() const { return state; }
	void launch();
	void onLanded();
	void update(float dt);
	void newTarget(Vector2 targetPosition);

protected:

	void removeMesh();
	void updateState();
	void updatePosition(float dt);
	void updateFlightParams(float dt);
	void setState(AicraftState newState);
	bool isTimeToGoToBase() const;
	void adjustTrajectoryToTarget(Vector2 target);
	void adjustTrajectoryToMoveAroundTarget(Vector2 target);

protected:

	scene::Mesh *mesh = nullptr;
	Ship *ship = nullptr;
	int number = 0;
	AicraftState state = AicraftState::NotReady;
	Vector2 position;
	float shipPosition = 0;
	Vector2 target;
	std::chrono::time_point<std::chrono::system_clock> nextStateTime;
	float angle = 0;
	float speed = 0;
	float acceleration = 0;
	float angularSpeed = 0;
	float flybyRadius = 0;
};
