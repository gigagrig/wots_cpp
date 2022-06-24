#include "ship.hpp"

#include <cassert>
#include <cmath>

Ship::Ship() :
	mesh(nullptr)
{
}

void Ship::init()
{
	assert(!mesh);
	mesh = scene::createShipMesh();
	position = Vector2(0.f, 0.f);
	angle = 0.f;
	aicrafts = std::list<AicraftPtr>(5);
	int sideNumber = 0;
	for (auto& craft: aicrafts)
	{
		craft = std::make_unique<Aicraft>();
		craft->init(this, ++sideNumber);
	}
	for (bool &key : input)
		key = false;
}


void Ship::deinit()
{
	scene::destroyMesh(mesh);
	mesh = nullptr;
}


void Ship::update(float dt)
{
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;

	if (input[game::KEY_FORWARD])
	{
		linearSpeed = params::ship::LINEAR_SPEED;
	}
	else if (input[game::KEY_BACKWARD])
	{
		linearSpeed = -params::ship::LINEAR_SPEED;
	}

	if (input[game::KEY_LEFT] && linearSpeed != 0.f)
	{
		angularSpeed = params::ship::ANGULAR_SPEED;
	}
	else if (input[game::KEY_RIGHT] && linearSpeed != 0.f)
	{
		angularSpeed = -params::ship::ANGULAR_SPEED;
	}

	angle = angle + angularSpeed * dt;
	position = position + linearSpeed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
	for(auto& aicraft : aicrafts)
		aicraft->update(dt);
}


void Ship::keyPressed(int key)
{
	assert(key >= 0 && key < game::KEY_COUNT);
	input[key] = true;
}


void Ship::keyReleased(int key)
{
	assert(key >= 0 && key < game::KEY_COUNT);
	input[key] = false;
}


void Ship::mouseClicked(Vector2 worldPosition, bool isLeftButton)
{
	// TODO: placeholder, remove it and implement aircarfts logic
	if (isLeftButton)
	{
		scene::placeGoalMarker(worldPosition.x, worldPosition.y);
		for (auto& aicraft : aicrafts)
			aicraft->newTarget(worldPosition);
	}
	else
	{
		tryLaunchAicraft();
	}
}


#include <iostream>
void Ship::tryLaunchAicraft()
{
	if (! (aicrafts.front()->getState() == AicraftState::Ready))
	{
		game::log(game::LOG_INFO, "There are no ready aicrafts");
		return;
	}

	auto craft = std::move(aicrafts.front());
	aicrafts.pop_front();
	aicrafts.push_back(std::move(craft));
	aicrafts.back()->launch();
}
