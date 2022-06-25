#pragma once

#include "../framework/scene.hpp"
#include "../framework/game.hpp"
#include "traits.hpp"

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
	MovingToTarget,
	MovingToBase
};

enum class AicraftTakticalTask
{
	No = 0,
	Takeoff,
	MovingToTraget
};

class Aicraft
{

public:
	Aicraft();
	~Aicraft();


	void init(Ship *ship, int sideNumber);
	void deinit();

	AicraftState getState() { return state; }


	void launch();
	void onLanded();
	void update(float dt);
	void newTarget(Vector2 worldPosition);

	bool onTakeoff();

protected:
	
	void updateState();
	bool isTimeToGoToBase();

protected:
	scene::Mesh *mesh;
	Ship *ship;
	int number;

	AicraftState state;
	AicraftTakticalTask takticalTask;

	Vector2 position;
	Vector2 target;
	std::chrono::time_point<std::chrono::system_clock> nextStateTime;
	float angle = 0;
	float speed = 0;
	float acceleration = 0;
	float angularSpeed = 0;
};
