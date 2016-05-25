/*
* Logger.cpp
*
* Created: 5/24/2016 03:00:28 PM
*  Author: Alex St. Clair
*/

#include "util/Logger.h"

//using namespace Phoenix::HAL;
using namespace Phoenix::Core;

Logger::Logger(void)
{
	//Left intentionally blank
}

Logger::~Logger(void)
{
	//Left intentionally blank
}

void Logger::Initialize(void)
{
	//Left intentionally blank
}

bool Logger::IsFullyInitialized(void)
{
        return (Singleton::IsFullyInitialized());
}

void Log(char * message, Logger_Level level_in)
{
	if(level_in >= 0){
		char * level;
		switch (level_in)
		{
			case LEVEL_DEBUG:
				level = (char *) "DBG";
				break;
			case LEVEL_INFO:
				level = (char *) "INF";
				break;
			case LEVEL_WARNING:
				level = (char *) "WRN";
				break;
			case LEVEL_ERROR:
				level = (char *) "ERR";
				break;
			case LEVEL_CRITICAL:
				level = (char *) "CRT";
				break;
			default:
				//oops
				break;
		}

		printf(level);
		printf(": ");
		printf(message);
		printf("\n");
	}
}
