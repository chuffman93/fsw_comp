/*
 * 	Logger.cpp
 *
 * 	Created on: May 26, 2016
 * 	    Author: Alex St. Clair
 */

#include "core/StdTypes.h"
#include "util/Logger.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// Set desired threshold for logging here
const LoggerLevelType Logger::threshold = LOGGER_LEVEL_DEBUG;
using namespace AllStar::Core;
using namespace std;


void Logger::Initialize(void){
	printf("\n-------------- Logger initialized with threshold: %d --------------\n\n", threshold);
}

bool Logger::IsFullyInitialized(void)
{
        return (Singleton::IsFullyInitialized());
}

Logger::Logger(void){
	//Left intentionally blank
}

Logger::~Logger(void){
	//Left intentionally blank
}

// TODO:
// 1. X When writing out a logging line, prepend a timestamp to the line
// 2. X When writing out a logging line, derive a thread ID and write it out, probably between the timestamp and the actual logging text
// 3. X The logging method should have two parameters:  a string and the logging level, like "DEBUG" or "INFO" or "ERROR".  The logging level should be an enumerated type and part of the logging class.
// 4.   The logging class can output to the screen, a file, or both
// 5.   If logging to a file, the PID and process name should be part of the filename
void Logger::Log(char const * message, LoggerLevelType level_in){

	if(level_in >= threshold){
		GetInfo(level_in);

		// Display the message
		printf("%s\n", message);
	}
}

void Logger::Log(string message, LoggerLevelType level_in){
	Log(message.c_str(), level_in);
}

void Logger::Log(char const * message, int int_in, LoggerLevelType level_in){
	if(level_in >= threshold){
		GetInfo(level_in);

		// Display the message
		printf(message, int_in);
		printf("\n");
	}
}

void Logger::Log(char const * message, uint32 uint_in, LoggerLevelType level_in){
	if(level_in >= threshold){
		GetInfo(level_in);

		// Display the message
		printf(message, uint_in);
		printf("\n");
	}
}

void Logger::Log(char const * message, float float_in, LoggerLevelType level_in){
	if(level_in >= threshold){
		GetInfo(level_in);

		//Display the message
		printf(message, float_in);
		printf("\n");
	}
}

void Logger::Log(char const * message, const char * char_in, LoggerLevelType level_in){
	if(level_in >= threshold){
		GetInfo(level_in);

		// Display the message
		printf(message, char_in);
		printf("\n");
	}
}

void Logger::Log(char const * message, string str_in, LoggerLevelType level_in){
	Log(message, str_in.c_str(), level_in);
}

void Logger::Log(string message, string str_in, LoggerLevelType level_in){
	Log(message.c_str(), str_in.c_str(), level_in);
}

void Logger::GetInfo(LoggerLevelType level_in){
	printf("(%lld)", getTimeInMillis());
	printf("[TID: %lu] ", pthread_self());

	// Display the log level
	switch(level_in){
	case LOGGER_LEVEL_SUPER_DEBUG:
		printf("SUPER:  ");
		break;
	case LOGGER_LEVEL_DEBUG:
		printf("DEBUG:    ");
		break;
	case LOGGER_LEVEL_INFO:
		printf("INFO:     ");
		break;
	case LOGGER_LEVEL_WARN:
		printf("WARN:     ");
		break;
	case LOGGER_LEVEL_ERROR:
		printf("ERROR:    ");
		break;
	case LOGGER_LEVEL_FATAL:
		printf("FATAL:    ");
		break;
	default:
		// maybe add assert on bounds
		break;
	}
}
