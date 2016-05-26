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

// TODO:
//	1. 	 When writing out a logging line, prepend a timestamp to the line
//	2. 	 When writing out a logging line, derive a thread ID and write it out, probably between the timestamp and the actual logging text
//	3. 	 The logging method should have two parameters:  a string and the logging level, like "DEBUG" or "INFO" or "ERROR".
// 			The logging level should be an enumerated type and part of the logging class.
//	4. 	 The logging class can output to the screen, a file, or both
//	5. 	 If logging to a file, the PID and process name should be part of the filename
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

bool Test(bool in){
	return in;
}
