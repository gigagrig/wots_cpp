#pragma once

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

