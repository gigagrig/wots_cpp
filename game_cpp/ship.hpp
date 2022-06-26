#pragma once

#include "../framework/scene.hpp"
#include "../framework/game.hpp"
#include "aircraft.hpp"
#include "utils.hpp"

#include <list>



//-------------------------------------------------------
//	Simple ship logic
//-------------------------------------------------------

class Ship
{
public:
	Ship();

	void init();
	void deinit();
	void update(float dt);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseClicked(Vector2 worldPosition, bool isLeftButton);

	const Vector2& getPosition() const { return position; }
	float getAngle() const { return angle; }
	float getSpeed() const { return linearSpeed; }
	float getAngularSpeed() const { return angularSpeed; }

	Vector2 localToGlobal(float localPosition) const;
	bool isOnShip(float localPosition) const;

protected:
	void tryLaunchAicraft();

private:
	scene::Mesh *mesh;
	Vector2 position;
	float angle;
	float linearSpeed;
	float angularSpeed;

	bool input[game::KEY_COUNT];

	std::list<AicraftPtr> aicrafts;
	Vector2 target;
};