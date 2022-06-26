
#include <cassert>
#include <cmath>
#include <cstdio>

#include <windows.h>  // for logging only

#include "ship.hpp"


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

	void log(LogLevel level, const char* text)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
			switch (level)
			{
			case LOG_DEBUG:
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); // white
				break; 
			case LOG_INFO:
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); 
				break; 
			case LOG_ERROR:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED); 
				break; 
			default:
				break; 
			}
		printf(text); 
		printf("\n"); 
	}

}
