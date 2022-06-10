
#include <cassert>
#include <cmath>

#include "../framework/scene.hpp"
#include "../framework/game.hpp"


//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.5f;
		constexpr float ANGULAR_SPEED = 0.5f;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.f;
		constexpr float ANGULAR_SPEED = 2.5f;
	}
}


//-------------------------------------------------------
//	Basic Vector2 class
//-------------------------------------------------------

class Vector2
{
public:
	float x;
	float y;

	Vector2();
	Vector2( float vx, float vy );
	Vector2( Vector2 const &other );
};


Vector2::Vector2() :
	x( 0.f ),
	y( 0.f )
{
}


Vector2::Vector2( float vx, float vy ) :
	x( vx ),
	y( vy )
{
}


Vector2::Vector2( Vector2 const &other ) :
	x( other.x ),
	y( other.y )
{
}


Vector2 operator + ( Vector2 const &left, Vector2 const &right )
{
	return Vector2( left.x + right.x, left.y + right.y );
}


Vector2 operator * ( float left, Vector2 const &right )
{
	return Vector2( left * right.x, left * right.y );
}


//-------------------------------------------------------
//	Simple ship logic
//-------------------------------------------------------

class Ship
{
public:
	Ship();

	void init();
	void deinit();
	void update( float dt );
	void keyPressed( int key );
	void keyReleased( int key );
	void mouseClicked( Vector2 worldPosition, bool isLeftButton );

private:
	scene::Mesh *mesh;
	Vector2 position;
	float angle;

	bool input[ game::KEY_COUNT ];
};


Ship::Ship() :
	mesh( nullptr )
{
}


void Ship::init()
{
	assert( !mesh );
	mesh = scene::createShipMesh();
	position = Vector2( 0.f, 0.f );
	angle = 0.f;
	for ( bool &key : input )
		key = false;
}


void Ship::deinit()
{
	scene::destroyMesh( mesh );
	mesh = nullptr;
}


void Ship::update( float dt )
{
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;

	if ( input[ game::KEY_FORWARD ] )
	{
		linearSpeed = params::ship::LINEAR_SPEED;
	}
	else if ( input[ game::KEY_BACKWARD ] )
	{
		linearSpeed = -params::ship::LINEAR_SPEED;
	}

	if ( input[ game::KEY_LEFT ] && linearSpeed != 0.f )
	{
		angularSpeed = params::ship::ANGULAR_SPEED;
	}
	else if ( input[ game::KEY_RIGHT ] && linearSpeed != 0.f )
	{
		angularSpeed = -params::ship::ANGULAR_SPEED;
	}

	angle = angle + angularSpeed * dt;
	position = position + linearSpeed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
	scene::placeMesh( mesh, position.x, position.y, angle );
}


void Ship::keyPressed( int key )
{
	assert( key >= 0 && key < game::KEY_COUNT );
	input[ key ] = true;
}


void Ship::keyReleased( int key )
{
	assert( key >= 0 && key < game::KEY_COUNT );
	input[ key ] = false;
}


void Ship::mouseClicked( Vector2 worldPosition, bool isLeftButton )
{
	// TODO: placeholder, remove it and implement aircarfts logic
	if ( isLeftButton )
	{
		scene::placeGoalMarker( worldPosition.x, worldPosition.y );
	}
	else
	{
		scene::Mesh *mesh = scene::createAircraftMesh();
		scene::placeMesh( mesh, worldPosition.x, worldPosition.y, 0.f );
	}
}


//-------------------------------------------------------
//	game public interface
//-------------------------------------------------------

namespace game
{
	Ship ship;


	void init()
	{
		ship.init();
	}


	void deinit()
	{
		ship.deinit();
	}


	void update( float dt )
	{
		ship.update( dt );
	}


	void keyPressed( int key )
	{
		ship.keyPressed( key );
	}


	void keyReleased( int key )
	{
		ship.keyReleased( key );
	}


	void mouseClicked( float x, float y, bool isLeftButton )
	{
		Vector2 worldPosition( x, y );
		scene::screenToWorld( &worldPosition.x, &worldPosition.y );
		ship.mouseClicked( worldPosition, isLeftButton );
	}
}

