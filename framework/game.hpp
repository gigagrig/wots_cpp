#pragma once


//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.25f;
		constexpr float ANGULAR_SPEED = 0.25f;
		constexpr int AICRAFTS_COUNT = 5;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.5f;
		constexpr float ACCELERATION = 0.2f;
		constexpr float ANGULAR_SPEED = 0.25 * 2 * 3.14f;
		constexpr int FLIGHT_TIME_SEC = 120;
		constexpr int FUELING_TIME_SEC = 20;
	}
}

//-------------------------------------------------------
//	game public interface
//-------------------------------------------------------

namespace game
{
	void init();
	void deinit();
	void update( float dt );

	enum
	{
		KEY_FORWARD,
		KEY_BACKWARD,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_COUNT
	};

	void keyPressed( int key );
	void keyReleased( int key );
	void mouseClicked( float x, float y, bool isLeftButton );

	enum LogLevel
	{
		LOG_DEBUG,
		LOG_INFO,
		LOG_ERROR
	};

	void log(LogLevel level, const char* text);

#define GAME_LOG(level, format, ...) \
	{ \
		constexpr int BUF_SIZE = 1048;\
		char buffer[BUF_SIZE];\
		sprintf_s(buffer, format, __VA_ARGS__);\
		log(level, buffer);\
	}
}

