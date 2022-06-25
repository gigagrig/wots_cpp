#pragma once

#include "../framework/scene.hpp"
#include "../framework/game.hpp"
#include "aircraft.hpp"
#include "traits.hpp"

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

	const Vector2& GetPosition() { return position; }
	float GetAngle() { return angle; }

protected:
	void tryLaunchAicraft();

private:
	scene::Mesh *mesh;
	Vector2 position;
	float angle;

	bool input[game::KEY_COUNT];

	std::list<AicraftPtr> aicrafts;
	Vector2 target;
};